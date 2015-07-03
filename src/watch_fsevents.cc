#include <CoreServices/CoreServices.h>

#include "watch_fsevents.h"

class FSEventsWatchPluginImpl {
public:
    FSEventStreamRef stream;

public:
    CFRunLoopRef run_loop;

public:
    WatchCallback callback;
};

static void fsevents_callback(__unused ConstFSEventStreamRef stream,
    void* client_callback_info,
    size_t num_events,
    void* event_paths,
    __unused const FSEventStreamEventFlags event_flags[],
    __unused const FSEventStreamEventId event_ids[])
{
    FSEventsWatchPluginImpl* impl = static_cast<FSEventsWatchPluginImpl*>(client_callback_info);

    std::vector<std::string> paths;

    for (size_t i = 0; i < num_events; ++i) {
        const char* path = static_cast<const char**>(event_paths)[i];
        paths.emplace_back(path);
    }

    impl->callback(paths);
}

FSEventsWatchPlugin::FSEventsWatchPlugin(
    const std::vector<std::string>& paths, const WatchCallback& callback)
    : m_impl(new FSEventsWatchPluginImpl())
{

    CFMutableArrayRef paths_array = CFArrayCreateMutable(kCFAllocatorDefault, 0, nullptr);

    for (auto& path : paths) {
        CFStringRef path_str
            = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8);

        CFArrayAppendValue(paths_array, path_str);

        CFRelease(path_str);
    }

    FSEventStreamContext context = { 1, m_impl.get(), nullptr, nullptr, nullptr };

    FSEventStreamRef stream = FSEventStreamCreate(kCFAllocatorDefault,
        fsevents_callback,
        &context,
        paths_array,
        kFSEventStreamEventIdSinceNow,
        0.1,
        kFSEventStreamCreateFlagFileEvents);

    if (!stream) {
        throw std::runtime_error("Could not create FSEventStream");
    }

    m_impl->stream = stream;
    m_impl->run_loop = CFRunLoopGetCurrent();
    m_impl->callback = callback;
}

FSEventsWatchPlugin::~FSEventsWatchPlugin()
{
    FSEventStreamStop(m_impl->stream);

    FSEventStreamUnscheduleFromRunLoop(m_impl->stream, m_impl->run_loop, kCFRunLoopCommonModes);

    FSEventStreamInvalidate(m_impl->stream);

    FSEventStreamRelease(m_impl->stream);
}

void FSEventsWatchPlugin::start()
{
    FSEventStreamScheduleWithRunLoop(m_impl->stream, m_impl->run_loop, kCFRunLoopCommonModes);

    Boolean started = FSEventStreamStart(m_impl->stream);

    if (!started) {
        throw std::runtime_error("Could not start FSEventStream");
    }

    CFRunLoopRun();
}

void FSEventsWatchPlugin::stop() { CFRunLoopStop(m_impl->run_loop); }

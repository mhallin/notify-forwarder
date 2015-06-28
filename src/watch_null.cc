#include "watch_null.h"

#include "platform.h"

#include <condition_variable>

class NullWatchPluginImpl {
    public: std::mutex wait_mutex;
    public: std::condition_variable cv;
};

NullWatchPlugin::NullWatchPlugin(
    __unused const std::vector<std::string>& paths,
    __unused const WatchCallback& callback)
: m_impl(new NullWatchPluginImpl()) {
}

NullWatchPlugin::~NullWatchPlugin() {
}

void NullWatchPlugin::start() {
    std::unique_lock<std::mutex> lock(m_impl->wait_mutex);
    m_impl->cv.wait(lock);
}

void NullWatchPlugin::stop() {
    m_impl->cv.notify_all();
}

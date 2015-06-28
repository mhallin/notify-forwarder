#include "watch_null.h"

#include "platform.h"

#include <condition_variable>

class NullWatchPluginImpl {
};

NullWatchPlugin::NullWatchPlugin(
    __unused const std::vector<std::string>& paths,
    __unused const WatchCallback& callback)
: m_impl(new NullWatchPluginImpl()) {
}

NullWatchPlugin::~NullWatchPlugin() {
}

void NullWatchPlugin::start() {
}

void NullWatchPlugin::stop() {
}

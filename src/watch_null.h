#pragma once

#include <memory>

#include "watch.h"

class NullWatchPluginImpl;

class NullWatchPlugin : public IWatchPlugin {
    public: NullWatchPlugin(
        const std::vector<std::string>& paths,
        const WatchCallback& callback);
    public: ~NullWatchPlugin();

    public: virtual void start();
    public: virtual void stop();

    private: std::unique_ptr<NullWatchPluginImpl> m_impl;
};

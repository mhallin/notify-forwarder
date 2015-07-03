#pragma mark

#include <memory>

#include <string>
#include <vector>

#include "watch.h"

class InotifyWatchPluginImpl;

class InotifyWatchPlugin : public IWatchPlugin {
public:
    InotifyWatchPlugin(const std::vector<std::string>& paths, const WatchCallback& callback);

public:
    ~InotifyWatchPlugin();

public:
    virtual void start();

public:
    virtual void stop();

private:
    std::unique_ptr<InotifyWatchPluginImpl> m_impl;
};

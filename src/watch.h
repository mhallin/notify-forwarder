#pragma once

#include <functional>
#include <string>
#include <vector>

typedef std::function<void(std::vector<std::string>)> WatchCallback;

class IWatchPlugin {
    // public: IWatchPlugin(
    //     const std::vector<std::string>& paths,
    //     const WatchCallback& callback)
public:
    virtual ~IWatchPlugin() {}

public:
    virtual void start() = 0;

public:
    virtual void stop() = 0;
};

#pragma once

#include <functional>
#include <string>
#include <vector>

typedef std::function<void(std::vector<std::string>)> ReceiveCallback;

class IReceivePlugin {
public:
    virtual ~IReceivePlugin() {}

public:
    virtual void start() = 0;

public:
    virtual void stop() = 0;
};

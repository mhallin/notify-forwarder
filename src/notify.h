#pragma once

#include <vector>
#include <string>

class INotifyPlugin {
public:
    virtual ~INotifyPlugin() {}

public:
    virtual void notify(const std::vector<std::string>& paths) = 0;
};

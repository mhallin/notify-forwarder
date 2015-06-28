#pragma once

#include <vector>
#include <string>

class IInjectPlugin {
    public: virtual ~IInjectPlugin() { }

    public: virtual void inject(const std::vector<std::string>& paths) = 0;
};

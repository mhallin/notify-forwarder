#pragma once

#include "inject.h"

class UtimesInjectPlugin : public IInjectPlugin {
    public: virtual ~UtimesInjectPlugin();

    public: virtual void inject(const std::vector<std::string>& paths);
};

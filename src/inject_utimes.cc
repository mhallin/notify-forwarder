#include <sys/time.h>

#include <stdexcept>

#include "inject_utimes.h"

UtimesInjectPlugin::~UtimesInjectPlugin() {
}

void UtimesInjectPlugin::inject(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        utimes(path.c_str(), nullptr);
    }
}

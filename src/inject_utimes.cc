#include <sys/time.h>

#include <stdexcept>

#include "inject_utimes.h"

UtimesInjectPlugin::~UtimesInjectPlugin() {
}

void UtimesInjectPlugin::inject(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        int result = utimes(path.c_str(), nullptr);

        if (result != 0) {
            fprintf(stderr, "Could not notify on file %s, file must have disappeared\n", path.c_str());
        }
    }
}

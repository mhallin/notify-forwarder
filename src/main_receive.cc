#include <stdlib.h>
#include <sysexits.h>

#include "main.h"
#include "utils.h"

#include "receive.h"
#include "receive_udp.h"

#include "inject.h"
#include "inject_utimes.h"

#include "platform.h"

static IReceivePlugin* gCurrentReceiveInstance = nullptr;

static void handle_sigint(__unused int signal) {
    if (gCurrentReceiveInstance) {
        gCurrentReceiveInstance->stop();
    }
    else {
        exit(EX_OK);
    }
}

int main_receive(__unused char* argv0, __unused int argc, __unused char** argv) {
    UtimesInjectPlugin injector;

    ReceiveCallback callback = [&injector](std::vector<std::string> paths) {
        fprintf(stderr, "Got event\n");

        for (auto& path : paths) {
            fprintf(stderr, "Path: %s\n", path.c_str());
        }

        injector.inject(paths);
    };

    UDPReceivePlugin receive_plugin(callback);
    SignalOverride(SIGINT, handle_sigint);
    VariableOverrideGuard<IReceivePlugin*> var_guard(gCurrentReceiveInstance, &receive_plugin);

    receive_plugin.start();

    return EX_OK;
}

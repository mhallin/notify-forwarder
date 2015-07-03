#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "main.h"
#include "utils.h"

#include "receive.h"
#include "receive_udp.h"

#include "inject.h"
#include "inject_utimes.h"

#include "platform.h"

static IReceivePlugin* gCurrentReceiveInstance = nullptr;

static void handle_sigint(__unused int signal)
{
    if (gCurrentReceiveInstance) {
        gCurrentReceiveInstance->stop();
    }
    else {
        exit(EX_OK);
    }
}

static void print_usage(char* argv0)
{
    fprintf(stderr, "Usage: %s receive [-p port]\n", argv0);
    fprintf(stderr, "\n");
    fprintf(stderr, "  -p port  specifies which ports to listen to. Defaults to 29324\n");
    fprintf(stderr, "\n");
}

int main_receive(char* argv0, int argc, char** argv)
{
    UtimesInjectPlugin injector;

    short port = 29324;

    if (argc >= 2 && strcmp(argv[0], "-p") == 0) {
        port = static_cast<short>(atoi(argv[1]));

        argc -= 2;
        argv += 2;
    }

    if (argc) {
        print_usage(argv0);
        return EX_USAGE;
    }

    ReceiveCallback callback
        = [&injector](std::vector<std::string> paths) { injector.inject(paths); };

    UDPReceivePlugin receive_plugin(port, callback);
    SignalOverride(SIGINT, handle_sigint);
    VariableOverrideGuard<IReceivePlugin*> var_guard(gCurrentReceiveInstance, &receive_plugin);

    receive_plugin.start();

    return EX_OK;
}

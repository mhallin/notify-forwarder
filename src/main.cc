#include <string.h>
#include <sysexits.h>
#include <stdio.h>

#include "main.h"

static void print_usage(char* argv0) {
    fprintf(stderr, "Usage: %s (watch|receive)\n", argv0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EX_USAGE;
    }

    if (strcmp(argv[1], "watch") == 0) {
        return main_watch(argv[0], argc - 2, argv + 2);
    }
    else if (strcmp(argv[1], "receive") == 0) {
        return main_receive(argv[0], argc - 2, argv + 2);
    }
    else {
        print_usage(argv[0]);
        return EX_USAGE;
    }
}

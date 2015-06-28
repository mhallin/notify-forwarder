#include <stdlib.h>
#include <sys/param.h>
#include <sysexits.h>

#include <chrono>
#include <unordered_map>

#include "main.h"
#include "utils.h"

#include "watch.h"
#include "watch_fsevents.h"
#include "watch_null.h"

#include "notify.h"
#include "notify_udp.h"

#include "platform.h"

typedef std::chrono::system_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;
typedef std::unordered_map<std::string, TimePoint> FeedbackMap;
typedef std::vector<std::pair<std::string, std::string>> PrefixMap;

static IWatchPlugin *gCurrentWatchInstance = nullptr;

static void handle_sigint(__unused int signal) {
    if (gCurrentWatchInstance) {
        gCurrentWatchInstance->stop();
    }
    else {
        exit(EX_OK);
    }
}

static void print_usage(char *argv0) {
    fprintf(stderr, "Usage: %s watch (<from> <to>)...\n", argv0);
}

static std::string replace_prefix(const PrefixMap &prefix_map, std::string src) {
    for (const auto& pair : prefix_map) {

        if (src.substr(0, pair.first.size()) == pair.first) {
            return pair.second + src.substr(pair.first.size());
        }
    }

    return src;
}

static void clear_expired(FeedbackMap &feedback_map) {
    TimePoint now = Clock::now();

    for (auto it = feedback_map.begin(); it != feedback_map.end();) {
        if (it->second < now) {
            it = feedback_map.erase(it);
        }
        else {
            ++it;
        }
    }
}

static std::vector<std::string> filter_feedback(FeedbackMap &feedback_map,
                                                std::vector<std::string> paths) {
    clear_expired(feedback_map);

    std::vector<std::string> out_paths;

    for (const auto& path : paths) {
        if (!feedback_map.count(path)) {
            out_paths.emplace_back(path);
        }

        feedback_map[path] = Clock::now() + std::chrono::seconds(1);
    }

    return out_paths;
}

int main_watch(char *argv0, int argc, char** argv) {
    std::vector<std::string> watch_paths;
    std::vector<std::pair<std::string, std::string>> prefix_map;
    FeedbackMap feedback_map;

    if (argc % 2 != 0 || argc == 0) {
        print_usage(argv0);
        return EX_USAGE;
    }

    for (int i = 0; i < argc; i += 2) {
        char out_path[MAXPATHLEN];
        char* in_path = argv[i];

        if (!realpath(in_path, out_path)) {
            throw std::runtime_error("Could not resolve path");
        }

        fprintf(stderr, "Converted %s to %s\n", in_path, out_path);

        watch_paths.emplace_back(out_path);
        prefix_map.emplace_back(out_path, argv[i+1]);
    }

    UDPNotifyPlugin notify_plugin;

    WatchCallback callback = [&notify_plugin, &prefix_map, &feedback_map](std::vector<std::string> paths) {
        std::vector<std::string> out_paths;

        for (auto& path : paths) {
            out_paths.emplace_back(replace_prefix(prefix_map, path));
        }

        notify_plugin.notify(filter_feedback(feedback_map, out_paths));
    };

    WATCH_PLUGIN_TYPE plugin(watch_paths, callback);
    SignalOverride(SIGINT, handle_sigint);
    VariableOverrideGuard<IWatchPlugin*> var_guard(gCurrentWatchInstance, &plugin);

    plugin.start();

    return EX_OK;
}

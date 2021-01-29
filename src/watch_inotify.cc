#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <unordered_map>

#include "watch_inotify.h"

#define READ_BUF_LEN (1024 * (sizeof(inotify_event) + 16))

typedef std::unordered_map<int, std::string> DirectoryMap;

namespace {

void add_directory_watch(int fd, const std::string& root, DirectoryMap& directory_map)
{
    int result = inotify_add_watch(fd,
        root.c_str(),
        IN_CREATE | IN_MODIFY | IN_DELETE | IN_ATTRIB | IN_MOVED_TO | IN_MOVED_FROM);

    if (result == -1) {
        throw std::runtime_error("Could not add watch");
    }

    directory_map[result] = root;

    DIR* dir = opendir(root.c_str());
    if (!dir) {
        throw std::runtime_error("Could not open directory");
    }

    dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string path = root + "/" + entry->d_name;

        if (entry->d_type & DT_DIR) {
            add_directory_watch(fd, path, directory_map);
        }
        else if (entry->d_type == DT_UNKNOWN) {
            struct stat stat_result;
            int sresult = stat(path.c_str(), &stat_result);

            if (sresult != 0) {
				struct stat lstat_result;
				int lsresult = lstat(path.c_str(), &lstat_result);

				if (lsresult !=0) {
					throw std::logic_error("Could not stat file of unknown type");
				}
				// It's a dangling symlink, move on.
            }

            if (stat_result.st_mode & S_IFDIR) {
                add_directory_watch(fd, path, directory_map);
            }
        }
    }
    closedir(dir);
}
}

class InotifyWatchPluginImpl {
public:
    int fd;

public:
    std::vector<std::string> paths;

public:
    WatchCallback callback;

public:
    bool running;
};

InotifyWatchPlugin::InotifyWatchPlugin(
    const std::vector<std::string>& paths, const WatchCallback& callback)
    : m_impl(new InotifyWatchPluginImpl())
{
    m_impl->fd = inotify_init();
    if (m_impl->fd == -1) {
        throw std::runtime_error("Could not initialize inotify");
    }

    m_impl->paths = paths;
    m_impl->running = true;
    m_impl->callback = callback;
}

InotifyWatchPlugin::~InotifyWatchPlugin()
{
    if (m_impl->fd == -1) {
        close(m_impl->fd);
    }
}

void InotifyWatchPlugin::start()
{
    DirectoryMap directory_map;

    for (auto& path : m_impl->paths) {
        add_directory_watch(m_impl->fd, path, directory_map);
    }

    char buf[READ_BUF_LEN];

    timespec timeout;
    timeout.tv_sec = 0;
    timeout.tv_nsec = 100000000;

    sigset_t emptyset;
    if (sigemptyset(&emptyset) == -1) {
        throw std::logic_error("Could not clear out emptyset");
    }

    while (m_impl->running) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(m_impl->fd, &fds);

        int select_result = pselect(m_impl->fd + 1, &fds, nullptr, nullptr, &timeout, &emptyset);

        if (select_result == -1 && errno != EINTR) {
            throw std::runtime_error("Could not select on inotify file");
        }
        else if (select_result > 0) {
            int length = read(m_impl->fd, buf, sizeof(buf));

            std::vector<std::string> paths_to_notify;

            if (length == -1) {
                throw std::runtime_error("Could not read data from inotify file");
            }

            for (int i = 0; i < length;) {
                inotify_event* event = reinterpret_cast<inotify_event*>(&buf[i]);

                if (event->len) {
                    std::string root = directory_map.at(event->wd);
                    std::string path = root + "/" + event->name;

                    if (event->mask & IN_ISDIR) {
                        if (event->mask & IN_CREATE || event->mask & IN_MOVED_TO) {
                            add_directory_watch(m_impl->fd, path, directory_map);
                        }
                    }

                    paths_to_notify.emplace_back(path);
                }

                i += sizeof(inotify_event) + event->len;
            }

            m_impl->callback(paths_to_notify);
        }
    }
}

void InotifyWatchPlugin::stop() { m_impl->running = false; }

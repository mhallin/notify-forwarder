#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "notify_udp.h"

class UDPNotifyPluginImpl {
public:
    int socket_fd;

public:
    sockaddr_in dest_addr;
};

UDPNotifyPlugin::UDPNotifyPlugin(const std::string& addr, short port)
    : m_impl(new UDPNotifyPluginImpl())
{
    m_impl->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_impl->socket_fd == -1) {
        throw std::runtime_error("Could not create sending socket");
    }

    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    dest_addr.sin_port = htons(port);

    m_impl->dest_addr = dest_addr;
}

UDPNotifyPlugin::~UDPNotifyPlugin()
{
    if (m_impl->socket_fd >= 0) {
        close(m_impl->socket_fd);
    }
}

void UDPNotifyPlugin::notify(const std::vector<std::string>& paths)
{
    for (auto& path : paths) {
        ssize_t sent_bytes = sendto(m_impl->socket_fd,
            path.c_str(),
            path.size(),
            0,
            reinterpret_cast<sockaddr*>(&m_impl->dest_addr),
            sizeof(m_impl->dest_addr));

        if (sent_bytes == -1) {
            throw std::runtime_error("Failed to send UDP packet");
        }

        if (static_cast<size_t>(sent_bytes) != path.size()) {
            throw std::runtime_error("Failed to send entire path in packet");
        }
    }
}

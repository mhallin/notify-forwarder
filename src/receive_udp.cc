#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>

#include "receive_udp.h"

class UDPReceivePluginImpl {
public:
    ReceiveCallback callback;

public:
    int socket_fd;

public:
    bool running;
};

UDPReceivePlugin::UDPReceivePlugin(short port, const ReceiveCallback& callback)
    : m_impl(new UDPReceivePluginImpl())
{
    m_impl->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_impl->socket_fd == -1) {
        throw std::runtime_error("Could not create sending socket");
    }

    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    dest_addr.sin_port = htons(port);

    int bind_result
        = bind(m_impl->socket_fd, reinterpret_cast<sockaddr*>(&dest_addr), sizeof(dest_addr));

    if (bind_result != 0) {
        throw std::runtime_error("Could not bind listening socket");
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    int sockopt_result = setsockopt(m_impl->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    if (sockopt_result != 0) {
        throw std::runtime_error("Could not set socket timeout");
    }

    m_impl->callback = callback;
    m_impl->running = true;
}

UDPReceivePlugin::~UDPReceivePlugin()
{
    if (m_impl->socket_fd >= 0) {
        close(m_impl->socket_fd);
    }
}

void UDPReceivePlugin::start()
{
    char buffer[MAXPATHLEN];
    sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(src_addr);

    while (m_impl->running) {
        memset(&src_addr, 0, sizeof(src_addr));

        ssize_t bytes_read = recvfrom(m_impl->socket_fd,
            buffer,
            sizeof(buffer),
            0,
            reinterpret_cast<sockaddr*>(&src_addr),
            &src_addr_len);

        if (bytes_read == -1 && errno != EAGAIN) {
            throw std::runtime_error("Could not receive UDP message");
        }
        else if (bytes_read > 0) {
            std::string filename(buffer, static_cast<size_t>(bytes_read));
            m_impl->callback({ filename });
        }
    }
}

void UDPReceivePlugin::stop() { m_impl->running = false; }

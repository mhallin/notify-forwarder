#pragma once

#include <memory>

#include "receive.h"

class UDPReceivePluginImpl;

class UDPReceivePlugin : public IReceivePlugin {
public:
    UDPReceivePlugin(short port, const ReceiveCallback& callback);

public:
    virtual ~UDPReceivePlugin();

public:
    virtual void start();

public:
    virtual void stop();

private:
    std::unique_ptr<UDPReceivePluginImpl> m_impl;
};

#pragma once

#include <signal.h>

#include <stdexcept>

template<typename T>
class VariableOverrideGuard {
    public: VariableOverrideGuard(T& var, const T& value)
    : m_old_value(var), m_var(var) {
        m_var = value;
    }

    public: ~VariableOverrideGuard() {
        m_var = m_old_value;
    }

    private: T m_old_value;
    private: T& m_var;
};


typedef void (*SignalHandler)(int);


class SignalOverride {
    public: SignalOverride(int sig, SignalHandler handler)
    : m_sig(sig), m_old_handler(signal(sig, handler)) {
        if (m_old_handler == SIG_ERR) {
            throw std::logic_error("Could not override signal");
        }
    }

    public: ~SignalOverride() {
        if (m_old_handler != SIG_ERR) {
            signal(m_sig, m_old_handler);
        }
    }

    private: int m_sig;
    private: SignalHandler m_old_handler;
};

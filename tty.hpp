#pragma once

#include "handle.hpp"
#include "error.hpp"

namespace uvpp {
class TTY : public stream<uv_tty_t>
{
public:

    enum Type
    {
        STDIN,
        STDOUT,
        STDERR
    };

    TTY(Type type, bool readable):
        stream<uv_tty_t>(), type_(type)
    {
        uv_tty_init(uv_default_loop(), get(), static_cast<int>(type_), static_cast<int>(readable));
    }

    TTY(loop& l, Type type, bool readable):
        stream<uv_tty_t>(), type_(type)
    {
        uv_tty_init(l.get(), get(), static_cast<int>(type_), static_cast<int>(readable));
    }
private:
    Type type_;
};
}
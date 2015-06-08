#pragma once

#include "handle.hpp"
#include "error.hpp"

namespace uvpp {

class Signal : public handle<uv_signal_t>
{
public:
    
    typedef std::function<void(int sugnum)> SignalHandler;
    
    Signal():
        handle<uv_signal_t>()
    {
        uv_signal_init(uv_default_loop(), get());
    }

    Signal(loop& l):
        handle<uv_signal_t>()
    {
        uv_signal_init(l.get(), get());
    }


    error start(int signum, SignalHandler callback)
    {
        callbacks::store(get()->data, internal::uv_cid_signal, callback);
        return error(uv_signal_start(get(),
                                     [](uv_signal_t* handle, int signum)
        {
            callbacks::invoke<decltype(callback)>(handle->data, internal::uv_cid_signal, signum);
        },
        signum));
    }

    error stop()
    {
        return error(uv_signal_stop(get()));
    }
};
}

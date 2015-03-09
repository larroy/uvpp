#pragma once

#include "handle.hpp"
#include "error.hpp"
#include <chrono>
#include <iostream>

namespace uvpp
{
    class Async : public handle<uv_async_t>
    {
    public:
        Async(std::function<void()> callback):
            handle<uv_timer_t>()
        {
            init(uv_default_loop(), callback);
        }

        Async(loop &l, std::function<void()> callback):
            handle<uv_timer_t>()
        {
            init(l.get(), callback);
        }

        void init(uv_loop_t *l, std::function<void()> callback) {
            callbacks::store(get()->data, internal::uv_cid_async, callback);

            uv_async_init(l, get(), [](uv_async_t* handle){
                callbacks::invoke<decltype(callback)>(handle->data, internal::uv_cid_async);
            });            
        }

        error send() {
            return error(uv_asenc_send(get()));
        }
    };
}
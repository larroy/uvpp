#pragma once

#include "callback.hpp"
#include "error.hpp"

namespace uvpp {
namespace {
template<typename HANDLE_T>
inline void free_handle(HANDLE_T** h)
{
    if (*h == nullptr)
        return;

    if ((*h)->data)
    {
        delete reinterpret_cast<callbacks*>((*h)->data);
        (*h)->data = nullptr;
    }

    switch ((*h)->type)
    {
        case UV_TCP:
            delete reinterpret_cast<uv_tcp_t*>(*h);
            break;

        case UV_UDP:
            delete reinterpret_cast<uv_udp_t*>(*h);
            break;

        case UV_NAMED_PIPE:
            delete reinterpret_cast<uv_pipe_t*>(*h);
            break;

        case UV_TTY:
            delete reinterpret_cast<uv_tty_t*>(*h);
            break;

        case UV_TIMER:
            delete reinterpret_cast<uv_timer_t*>(*h);
            break;

        case UV_SIGNAL:
            delete reinterpret_cast<uv_signal_t*>(*h);
            break;

        case UV_POLL:
            delete reinterpret_cast<uv_poll_t*>(*h);
            break;

        case UV_ASYNC:
            delete reinterpret_cast<uv_async_t*>(*h);
            break;

        case UV_IDLE:
            delete reinterpret_cast<uv_idle_t*>(*h);
            break;

        case UV_FS_EVENT:
            delete reinterpret_cast<uv_fs_event_t*>(*h);
            break;

        default:
            assert(0);
            throw std::runtime_error("free_handle can't handle this type");
            break;
            *h = nullptr;
    }
}

}

/**
 * Wraps a libuv's uv_handle_t, or derived such as uv_stream_t, uv_tcp_t etc.
 *
 * Resources are released on the close call as mandated by libuv and NOT on the dtor
 */
template<typename HANDLE_T>
class handle
{
protected:
    handle():
        m_uv_handle(new HANDLE_T())
        , m_will_close(false)
    {
        assert(m_uv_handle);
        m_uv_handle->data = new callbacks();
        assert(m_uv_handle->data);
    }

    handle(handle&& other):
        m_uv_handle(other.m_uv_handle)
        , m_will_close(other.m_will_close)
    {
        other.m_uv_handle = nullptr;
        other.m_will_close = false;
    }

    handle& operator=(handle&& other)
    {
        if (this == &other)
            return *this;
        m_uv_handle = other.m_uv_handle;
        m_will_close = other.m_will_close;
        other.m_uv_handle = nullptr;
        other.m_will_close = false;
        return *this;
    }

    virtual ~handle()
    {
        if (! m_will_close)
            free_handle(&m_uv_handle);
    }

    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

public:
    template<typename T=HANDLE_T>
    T* get()
    {
        return reinterpret_cast<T*>(m_uv_handle);
    }

    template<typename T=HANDLE_T>
    const T* get() const
    {
        return reinterpret_cast<const T*>(m_uv_handle);
    }

    bool is_active() const
    {
        return uv_is_active(reinterpret_cast<const uv_handle_t*>(m_uv_handle)) != 0;
    }

    void close(Callback callback = [] {})
    {
        if (uv_is_closing(get<uv_handle_t>()))
        {
            return; // prevent assertion on double close
        }

        callbacks::store(get()->data, internal::uv_cid_close, callback);
        m_will_close = true;
        uv_close(get<uv_handle_t>(),
                 [](uv_handle_t* h)
        {
            callbacks::invoke<decltype(callback)>(h->data, internal::uv_cid_close);
            free_handle(&h);
        });
    }

protected:
    HANDLE_T* m_uv_handle;
    bool m_will_close;
};

}


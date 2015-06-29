#pragma once

#include "callback.hpp"
#include "error.hpp"

namespace uvpp {
namespace {
template<typename REQUEST_T>
inline void free_request(REQUEST_T** h)
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

        case UV_WORK:
            delete reinterpret_cast<uv_work_t*>(*h);
            break;

        case UV_FS:
            delete reinterpret_cast<uv_fs_t*>(*h);
            break;
            
        case UV_GETADDRINFO:
            delete reinterpret_cast<uv_getaddrinfo_t*>(*h);
            break;
            
        default:
            assert(0);
            throw std::runtime_error("free_request can't handle this type");
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
template<typename REQUEST_T>
class request
{
protected:
    request():
        m_uv_request(new REQUEST_T())
        , m_will_close(false)
    {
        assert(m_uv_request);
        m_uv_request->data = new callbacks();
        assert(m_uv_request->data);
    }

    request(request&& other):
        m_uv_request(other.m_uv_request)
        , m_will_close(other.m_will_close)
    {
        other.m_uv_request = nullptr;
        other.m_will_close = false;
    }

    request& operator=(request&& other)
    {
        if (this == &other)
            return *this;
        m_uv_request = other.m_uv_request;
        m_will_close = other.m_will_close;
        other.m_uv_request = nullptr;
        other.m_will_close = false;
        return *this;
    }

    ~request()
    {
        if (! m_will_close)
            free_request(&m_uv_request);
    }

    request(const request&) = delete;
    request& operator=(const request&) = delete;

public:
    template<typename T=REQUEST_T>
    T* get()
    {
        return reinterpret_cast<T*>(m_uv_request);
    }

    template<typename T=REQUEST_T>
    const T* get() const
    {
        return reinterpret_cast<const T*>(m_uv_request);
    }

    int cancel()
    {
        return uv_cancel((uv_req_t*)get());
    }

protected:
    REQUEST_T* m_uv_request;
    bool m_will_close;
};

}
#pragma once

#include "handle.hpp"
#include "error.hpp"
#include <algorithm>
#include <memory>

namespace uvpp {
template<typename HANDLE_T>
class stream : public handle<HANDLE_T>
{
protected:
    stream():
        handle<HANDLE_T>()
    {}

public:
    bool listen(CallbackWithResult callback, int backlog=128)
    {
        callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_listen, callback);
        return uv_listen(handle<HANDLE_T>::template get<uv_stream_t>(), backlog, [](uv_stream_t* s, int status)
        {
            callbacks::invoke<decltype(callback)>(s->data, uvpp::internal::uv_cid_listen, error(status));
        }) == 0;
    }

    bool accept(stream& client)
    {
        return uv_accept(handle<HANDLE_T>::template get<uv_stream_t>(), client.handle<HANDLE_T>::template get<uv_stream_t>()) == 0;
    }

    bool read_start(std::function<void(const char* buf, ssize_t len)> callback)
    {
        return read_start<0>(callback);
    }

    template<size_t max_alloc_size>
    bool read_start(std::function<void(const char* buf, ssize_t len)> callback)
    {
        callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_read_start, callback);

        return uv_read_start(handle<HANDLE_T>::template get<uv_stream_t>(),
                             [](uv_handle_t*, size_t suggested_size, uv_buf_t* buf)
        {
            assert(buf);
            auto size = std::max(suggested_size, max_alloc_size);
            buf->base = new char[size];
            buf->len = size;
        },
        [](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf)
        {
            // handle callback throwing exception: hold data in unique_ptr
            std::shared_ptr<char> baseHolder(buf->base, std::default_delete<char[]>());

            if (nread < 0)
            {
                // FIXME error has nread set to -errno, handle failure
                // assert(nread == UV_EOF); ???
                callbacks::invoke<decltype(callback)>(s->data, uvpp::internal::uv_cid_read_start, nullptr, nread);
            }
            else if (nread >= 0)
            {
                callbacks::invoke<decltype(callback)>(s->data, uvpp::internal::uv_cid_read_start, buf->base, nread);
            }
        }) == 0;
    }

    bool read_stop()
    {
        return uv_read_stop(handle<HANDLE_T>::template get<uv_stream_t>()) == 0;
    }

    bool write(const char* buf, int len, CallbackWithResult callback)
    {
        uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf), static_cast<size_t>(len) } };
        callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_write, callback);
        return uv_write(new uv_write_t, handle<HANDLE_T>::template get<uv_stream_t>(), bufs, 1, [](uv_write_t* req, int status)
        {
            std::unique_ptr<uv_write_t> reqHolder(req);
            callbacks::invoke<decltype(callback)>(req->handle->data, uvpp::internal::uv_cid_write, error(status));
        }) == 0;
    }

    bool write(const std::string& buf, CallbackWithResult callback)
    {
        uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf.c_str()), buf.length()} };
        callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_write, callback);
        return uv_write(new uv_write_t, handle<HANDLE_T>::template get<uv_stream_t>(), bufs, 1, [](uv_write_t* req, int status)
        {
            std::unique_ptr<uv_write_t> reqHolder(req);
            callbacks::invoke<decltype(callback)>(req->handle->data, uvpp::internal::uv_cid_write, error(status));
        }) == 0;
    }

    bool write(const std::vector<char>& buf, CallbackWithResult callback)
    {
        uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(&buf[0]), buf.size() } };
        callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_write, callback);
        return uv_write(new uv_write_t, handle<HANDLE_T>::template get<uv_stream_t>(), bufs, 1, [](uv_write_t* req, int status)
        {
            std::unique_ptr<uv_write_t> reqHolder(req);
            callbacks::invoke<decltype(callback)>(req->handle->data, uvpp::internal::uv_cid_write, error(status));
        }) == 0;
    }

    bool shutdown(CallbackWithResult callback)
    {
        callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_shutdown, callback);
        return uv_shutdown(new uv_shutdown_t, handle<HANDLE_T>::template get<uv_stream_t>(), [](uv_shutdown_t* req, int status)
        {
            std::unique_ptr<uv_shutdown_t> reqHolder(req);
            callbacks::invoke<decltype(callback)>(req->handle->data, uvpp::internal::uv_cid_shutdown, error(status));
        }) == 0;
    }
};
}

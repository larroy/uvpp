#pragma once

#include <vector>
#include <functional>
#include <memory>
#include "error.hpp"

namespace uvpp {
typedef std::function<void()> Callback;
typedef std::function<void(error)> CallbackWithResult;

namespace internal {
enum uv_callback_id
{
    uv_cid_close = 0,
    uv_cid_listen,
    uv_cid_read_start,
    uv_cid_write,
    uv_cid_shutdown,
    uv_cid_connect,
    uv_cid_connect6,
    uv_cid_work,
    uv_cid_after_work,
    uv_cid_timer,
    uv_cid_poll,
    uv_cid_signal,
    uv_cid_async,
    uv_cid_idle,
    uv_cid_fs_open,
    uv_cid_fs_read,
    uv_cid_fs_write,
    uv_cid_fs_close,
    uv_cid_fs_unlink,
    uv_cid_fs_stats,
    uv_cid_fs_fsync,
    uv_cid_fs_rename,
    uv_cid_fs_sendfile,
    uv_cid_fs_poll,
    uv_cid_fs_event,
    uv_cid_fs_scandir,
    uv_cid_resolve,
    uv_cid_max
};

class callback_object_base
{
public:
    callback_object_base(void* data):
        m_data(data)
    {
    }

    callback_object_base(callback_object_base&&) = default;
    callback_object_base& operator=(callback_object_base&&) = default;
    callback_object_base(const callback_object_base&) = default;
    callback_object_base& operator=(const callback_object_base&) = default;

    virtual ~callback_object_base()
    {
    }

    void* get_data() const
    {
        return m_data;
    }

private:
    void* m_data;
};

template<typename callback_t>
class callback_object : public callback_object_base
{
public:
    callback_object(const callback_t& callback, void* data=nullptr)
        : callback_object_base(data)
        , m_callback(callback)
    {
    }

public:
    template<typename ...A>
    typename std::result_of<callback_t(A...)>::type invoke(A&& ... args)
    {
        return m_callback(std::forward<A>(args)...);
    }

private:
    callback_t m_callback;
};
} // end ns internals

typedef std::unique_ptr<internal::callback_object_base> callback_object_ptr;

/**
 * Class that allows to install callback objects for each uv_callback_id value taking ownership
 * of the callback object which is copied.
 */
class callbacks
{
public:
    callbacks():
        m_lut(internal::uv_cid_max)
    {
    }

    callbacks(int max):
        m_lut(max)
    {
    }

    template<typename callback_t>
    static void store(void* target, int cid, const callback_t& callback, void* data=nullptr)
    {
        reinterpret_cast<callbacks*>(target)->m_lut[cid] = callback_object_ptr(new internal::callback_object<callback_t>(callback, data));
    }

    template<typename callback_t>
    static void* get_data(void* target, int cid)
    {
        return reinterpret_cast<callbacks*>(target)->m_lut[cid]->get_data();
    }

    template<typename callback_t, typename ...A>
    static typename std::result_of<callback_t(A...)>::type invoke(void* target, int cid, A&& ... args)
    {
        auto x = dynamic_cast<internal::callback_object<callback_t>*>(reinterpret_cast<callbacks*>(target)->m_lut[cid].get());
        assert(x);
        return x->invoke(std::forward<A>(args)...);
    }

private:
    std::vector<callback_object_ptr> m_lut;
};
}


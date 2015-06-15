#pragma once

#include "handle.hpp"
#include "error.hpp"
#include "file.hpp"
#include <iostream>

namespace uvpp {

inline bool fileExists(const char *path)
{
    struct stat   buffer;
    return (stat (path, &buffer) == 0);
}

class FsEvent : public handle<uv_fs_event_t>
{
public:

    enum Type
    {
        NONE,
        CHANGED,
        CREATED,
        DELETED
    };

    FsEvent():
        handle<uv_fs_event_t>()
    {
        uv_fs_event_init(uv_default_loop(), get());
    }

    FsEvent(loop& l):
        handle<uv_fs_event_t>()
    {
        uv_fs_event_init(l.get(), get());
    }

    ~FsEvent()
    {
        if (started_) stop();
    }

    error start(const std::string &path, unsigned int flags, std::function<void(error err,const std::string &path, int status,Type event)> callback)
    {

        auto fsEventCallback = [this,path,callback](const char* filename, int events, int status)
        {
            if (status<0)
                callback(error(status), std::string(filename), status, NONE);
            else
            {

                if (events & UV_CHANGE)
                {
                    callback(error(status), std::string(filename), status, CHANGED);
                }

                if (events & UV_RENAME)
                {
                    std::string fullPath = path;
                    fullPath += filename;

                    if (fileExists(fullPath.c_str()))
                    {
                        callback(error(status), std::string(filename), status, CREATED);
                    }
                    else
                    {
                        callback(error(status), std::string(filename), status, DELETED);
                    }
                }
            }
        };

        callbacks::store(get()->data, internal::uv_cid_fs_event, fsEventCallback);

        started_ = true;
        return error(uv_fs_event_start(get(),
                                       [](uv_fs_event_t* handle, const char* filename, int events, int status)
        {
            callbacks::invoke<decltype(fsEventCallback)>(handle->data, internal::uv_cid_fs_event, filename, events,status);
        }, path.c_str(), flags
                                      ));
    }

    error stop()
    {
        return error(uv_fs_event_stop(get()));
    }

private:
    bool started_ = false;

};
}
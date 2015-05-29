#pragma once

#include "handle.hpp"
#include "error.hpp"

namespace uvpp
{

    class Signal : public handle<uv_signal_t>
    {
    public:	
    	Signal():
            handle<uv_signal_t>()
        {
            uv_signal_init(uv_default_loop(), get());
        }

        Signal(loop& l):
            handle<uv_poll_t>()
        {
            uv_signal_init(l.get(), get());
        }
             

        error start(std::function<void(int signum)> callback) {        	
			callbacks::store(get()->data, internal::uv_cid_signal, callback);
        	return error(uv_signal_start(get(),
        		[](uv_poll_t* handle, int status, int events){
        			callbacks::invoke<decltype(callback)>(handle->data, internal::uv_cid_signal, status, events);
        		}
        		));        	
        }

        error stop() {
			return error(uv_signal_stop(get()));            	
        }
    };
}    
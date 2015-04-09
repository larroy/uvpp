#pragma once

#include "request.hpp"
#include "error.hpp"

namespace uvpp
{
	class Work : public request<uv_work_t>
    {
    public:        
        Work(loop& l) : request<uv_work_t>(), loop_(l)
        {

        }

        bool execute(std::function<void()> callback, std::function<void(error)> afterCallback) {

        	
        	callbacks::store(get()->data, internal::uv_cid_work, callback);
        	callbacks::store(get()->data, internal::uv_cid_after_work, afterCallback);

        	return (
        		uv_queue_work(loop_.get(), get(),
        			[](uv_work_t* req) {
                		callbacks::invoke<decltype(callback)>(req->data, internal::uv_cid_work);
            		}, 
            		[](uv_work_t* req, int status) {
                		callbacks::invoke<decltype(afterCallback)>(req->data, internal::uv_cid_after_work, error(status));
            		}) == 0
            );
        }
    private:
    	loop &loop_;    	       	
    };
}

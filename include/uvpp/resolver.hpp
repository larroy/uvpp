#pragma once

#include "request.hpp"
#include "error.hpp"
#include "loop.hpp"

namespace uvpp {
    
class Resolver : public request<uv_getaddrinfo_t>
{
public:
    typedef std::function<void(const error&, const std::string&)> Callback; // status, addr
    Resolver(loop& l) : request<uv_getaddrinfo_t>(), loop_(l.get())
    {

    }
    bool resolve(const std::string& addr, Callback callback)
    {
        callbacks::store(get()->data, internal::uv_cid_resolve, callback);
        return (uv_getaddrinfo(loop_
                , get()
                , [](uv_getaddrinfo_t* req, int status, struct addrinfo* res)
                {
                    char addr[128] = {'\0'}; // address text buffer
                    if (status == 0)
                    {
                        if (res->ai_addrlen > 16) 
                        {   // ai_addrlen == 32 for ip6
                            uv_ip6_name(reinterpret_cast<struct sockaddr_in6*>(res->ai_addr), addr, res->ai_addrlen);
                        } else
                        {
                            uv_ip4_name(reinterpret_cast<struct sockaddr_in*>(res->ai_addr), addr, res->ai_addrlen);
                        }
                    }
                    callbacks::invoke<decltype(callback)>(req->data, internal::uv_cid_resolve, error(status), addr);
                }
                , addr.c_str(), 0, 0) == 0);
    }
private:
    uv_loop_t *loop_;
};

} // uvpp


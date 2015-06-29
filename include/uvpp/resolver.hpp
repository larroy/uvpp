#pragma once

#include "request.hpp"
#include "error.hpp"
#include "loop.hpp"

namespace uvpp {
    
class Resolver : public request<uv_getaddrinfo_t>
{
public:
    typedef std::function<void(const error&, bool, const std::string&)> Callback; // status, is_ip4, addr
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
                    std::shared_ptr<addrinfo> resHolder(res, [](addrinfo* res)
                    {
                        uv_freeaddrinfo(res);
                    });
                    char addr[128] = {'\0'}; // address text buffer
                    if (status == 0)
                    {
                        if (res->ai_family == AF_INET6) 
                        {
                            uv_ip6_name(reinterpret_cast<struct sockaddr_in6*>(res->ai_addr), addr, res->ai_addrlen);
                        } else if (res->ai_family == AF_INET)
                        {
                            uv_ip4_name(reinterpret_cast<struct sockaddr_in*>(res->ai_addr), addr, res->ai_addrlen);
                        } else
                        {
                            callbacks::invoke<decltype(callback)>(req->data, internal::uv_cid_resolve
                                , error(EAI_ADDRFAMILY)
                                , false
                                , addr);
                            return;
                        }
                    }
                    bool ip4 = res ? res->ai_family == AF_INET : false;
                    callbacks::invoke<decltype(callback)>(req->data, internal::uv_cid_resolve, error(status), ip4, addr);
                }
                , addr.c_str(), 0, 0) == 0);
    }
private:
    uv_loop_t *loop_;
};

} // uvpp


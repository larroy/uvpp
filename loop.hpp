#pragma once

#include "error.hpp"

namespace uvpp
{
    /**
     *  Class that represents the uv_loop instance.
     */
    class loop
    {
    public:
        /**
         *  Default constructor
         *  @param use_default indicates whether to use default loop or create a new loop.
         */
        loop(bool use_default=false):
            m_uv_loop(use_default ? uv_default_loop() : uv_loop_new())
        {
        }

        /**
         *  Destructor
         */
        ~loop()
        {
            if(m_uv_loop)
            {
                uv_loop_delete(m_uv_loop);
                m_uv_loop = nullptr;
            }
        }

        loop(const loop&) = delete;
        loop& operator=(const loop&) = delete;
        loop(loop&& other):
            m_uv_loop(other.m_uv_loop)
        {
            if (this != &other)
                other.m_uv_loop = nullptr;
        }

        loop& operator=(loop&& other)
        {
            if (this != &other)
            {
                m_uv_loop = other.m_uv_loop;
                other.m_uv_loop = nullptr;
            }
            return *this;
        }



        /**
         *  Returns internal handle for libuv functions.
         */
        uv_loop_t* get() { return m_uv_loop; }

        /**
         *  Starts the loop.
         */
        bool run()
        {
            return uv_run(m_uv_loop, UV_RUN_DEFAULT) == 0;
        }

        /**
         *  Polls for new events without blocking.
         */
        bool run_once()
        {
            return uv_run(m_uv_loop, UV_RUN_ONCE) == 0;
        }

        /**
         *  ...
         *  Internally, this function just calls uv_update_time() function.
         */
        void update_time() { uv_update_time(m_uv_loop); }

        /**
         *  ...
         *  Internally, this function just calls uv_now() function.
         */
        int64_t now() { return uv_now(m_uv_loop); }

    private:
        uv_loop_t* m_uv_loop;
    };

    /**
     *  Starts the default loop.
     */
    inline int run()
    {
        return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    }

    /**
     *  Polls for new events without blocking for the default loop.
     */
    inline int run_once()
    {
        return uv_run(uv_default_loop(), UV_RUN_ONCE);
    }
}


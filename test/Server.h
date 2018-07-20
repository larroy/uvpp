#pragma once
#include "TcpConnection.h"
#include <map>
#include <functional>
#include "uvpp/async.hpp"
#include <string>
#include <mutex>

struct msg_buf
{
	msg_buf(std::string p, std::string m) : peer(p), msg(m)
	{
	}

	std::string peer;
	std::string msg;
};

class Server
{
public:
	typedef std::function<void(const std::string &peer)> on_close_t;
	typedef std::function<void(const std::string &peer)> on_connect_t;
	Server();
	~Server();

	void set_port(int port);
	void start();
	void stop();

	void send(const std::string &peer, const std::string &msg);

private:
	void on_tcp_connect(uvpp::error error);
	void on_async();

private:
	int m_port;
	bool m_running;
	uvpp::loop m_loop;
	uvpp::Tcp m_tcp_listen_conn;

	on_close_t m_on_close;
	on_connect_t m_on_connect;

	uvpp::Async m_async;
	std::mutex m_send_lock;
	std::deque<msg_buf> m_sendBuf;

	/// connection identifier to Connection
	std::map<std::string, std::unique_ptr<TcpConnection>> m_connections;
};

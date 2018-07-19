#pragma once
#include "TcpConnection.h"
#include <map>

class Server
{
public:
	Server();
	~Server();

	void set_port(int port);
	void start();
	void stop();

private:
	void on_tcp_connect(uvpp::error error);

private:
	int m_port;
	bool m_running;
	uvpp::loop m_loop;
	uvpp::Tcp m_tcp_listen_conn;

	/// connection identifier to Connection
	std::map<std::string, std::unique_ptr<TcpConnection>> m_connections;
};

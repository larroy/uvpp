
#include "TcpConnection.h"
#include <iostream>
#include <string.h>

using namespace std;

void TcpConnection::set_peer_name(std::string &peer_name)
{
	m_peerName = peer_name;
}

/**
* Check if we have a full message then decode it and handle, otherwise wait for more data, same for
* payload.
*/
void TcpConnection::input(const char *data, size_t len)
{
	string buf;
	buf.resize(len);
	memcpy(&buf[0], data, len);
	buf.push_back('\0');
	cout << m_peerName << ":" << buf << " len:" << len << endl;
}

void TcpConnection::send_msg(const std::string &&msg)
{
	const bool do_write = m_output_buff.empty() == true;
	m_output_buff.emplace_back(move(msg));
	if (do_write)
		write_next_buff();
}

void TcpConnection::write_next_buff()
{
	assert(!m_write_in_progress);
	assert(!m_output_buff.empty());
	const string &buf = m_output_buff.front();
	m_do_write(buf.c_str(), buf.size());
	m_write_in_progress = true;
}

void TcpConnection::on_write_finished()
{
	m_write_in_progress = false;
	assert(!m_output_buff.empty());
	m_output_buff.pop_front();
	if (!m_output_buff.empty())
		write_next_buff();
}
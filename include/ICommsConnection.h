#pragma once
#include <boost/asio.hpp>

class ICommsConnection
{
public:
	~ICommsConnection() {}

	virtual boost::asio::generic::stream_protocol::socket &socket() = 0;
	virtual void start() = 0;
};

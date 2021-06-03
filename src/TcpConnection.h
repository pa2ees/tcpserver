#pragma once
#include "ICommsApp.h"
#include "ICommsConnection.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>


class TcpConnection : public ICommsConnection, public boost::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(boost::asio::io_context &ioContext, boost::shared_ptr<ICommsApp> commsApp = {});
	~TcpConnection();

	boost::asio::generic::stream_protocol::socket &socket() override;

	void start() override;

protected:
	void startRead();
	void writeMessage(std::string outgoingMessage);

	void handleWrite_(const boost::system::error_code & /*error*/, size_t /*bytes_transferred*/);
	void handleRead_(const boost::system::error_code &error, size_t bytesTransferred);

	boost::shared_ptr<ICommsApp> commsApp_;
	boost::asio::generic::stream_protocol::socket socket_;
	char incomingData_[1024];
	std::string incomingMessage_;
};

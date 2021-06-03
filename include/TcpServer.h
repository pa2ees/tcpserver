#pragma once
#include "ICommsApp.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// forward declare TcpConnection so it doesn't need to be in the public API
class TcpConnection;

class TcpServer
{
public:
	TcpServer(std::function<ICommsApp *()> getNewCommsApp, boost::shared_ptr<boost::asio::io_context> ioContext = {});
	~TcpServer();

	static boost::asio::generic::stream_protocol::endpoint createEndpoint();
	void run();
	void stop();

private:
	void startAccept();
	void handleAccept(boost::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error);


	// must be a function pointer that returns a ICommsApp* object
	std::function<ICommsApp *()> getNewCommsApp_;
	boost::thread *ioContextThread_ = nullptr;
	boost::shared_ptr<boost::asio::io_context> ioContext_;
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> tcpAcceptor_ = {};
	boost::shared_ptr<boost::asio::local::stream_protocol::acceptor> udsAcceptor_ = {};
	bool ioContextCreatedByUs_ = false;
};

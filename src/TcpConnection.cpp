#include "TcpConnection.h"
#include <boost/bind.hpp>
#include <iostream>
#include <string>


TcpConnection::TcpConnection(boost::asio::io_context &ioContext, boost::shared_ptr<ICommsApp> commsApp)
    : socket_(ioContext), commsApp_{commsApp}
{
}

TcpConnection::~TcpConnection()
{ // destructor
	std::cout << "TcpConnection out... *mic drop*" << std::endl;
}
boost::asio::generic::stream_protocol::socket &TcpConnection::socket()
{
	return socket_;
}

void TcpConnection::start()
{ // start up the read loop
	std::cout << "TcpConnection started" << std::endl;
	writeMessage("Thank you for connecting!\nIt will be my pleasure to respond to your every need\n");
	startRead();
}

void TcpConnection::startRead()
{
	std::cout << "Starting async read" << std::endl;
	socket_.async_read_some(boost::asio::buffer(incomingData_, 1024),
	                        boost::bind(&TcpConnection::handleRead_, shared_from_this(),
	                                    boost::asio::placeholders::error,
	                                    boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::writeMessage(std::string outgoingMessage)
{
	boost::asio::async_write(socket_, boost::asio::buffer(outgoingMessage),
	                         boost::bind(&TcpConnection::handleWrite_, shared_from_this(),
	                                     boost::asio::placeholders::error,
	                                     boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::handleWrite_(const boost::system::error_code &error, size_t bytesTransferred)
{
	if (commsApp_.get() != nullptr)
	{
		commsApp_->handleWrite(error, bytesTransferred);
	}
}
void TcpConnection::handleRead_(const boost::system::error_code &error, size_t bytesTransferred)
{
	std::cout << "Handling read" << std::endl;
	std::cout << "Error value: " << error.value() << std::endl;
	// no need to keep running if we have no commsApp to handle the reads
	bool keepRunning = false;

	std::string incomingMessage(incomingData_, bytesTransferred);
	if (commsApp_)
	{
		keepRunning = commsApp_->handleRead(incomingMessage, error, bytesTransferred) == 0;
	}
	else
	{
		std::cout << "We don't have a commsApp!" << std::endl;
	}

	if (error.value() == 2)
	{ // client disconnected
		std::cout << "Client disconnected" << std::endl;
		if (commsApp_)
		{
			std::cout << "Alerting commsApp that connection was closed" << std::endl;
			commsApp_->connectionClosed();
		}
		keepRunning = false;
	}

	std::cout << "keep running: " << (keepRunning ? "True" : "False") << std::endl;
	if (keepRunning)
	{ // we don't have a commsApp, or handleRead returned an error, or the client severed the connection
		startRead();
	}
}

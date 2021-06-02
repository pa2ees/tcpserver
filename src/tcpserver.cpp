// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <ctime>
#include <iostream>
#include <string>

bool running = true;
using boost::asio::ip::tcp;

std::string makeDaytimeString()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
public:
	typedef boost::shared_ptr<TcpConnection> pointer;

	static pointer create(boost::asio::io_context &io_context) { return pointer(new TcpConnection(io_context)); }

	tcp::socket &socket() { return socket_; }

	void start()
	{ // start up the read loop
		startRead();
	}

private:
	TcpConnection(boost::asio::io_context &io_context) : socket_(io_context) {}

	void startRead()
	{
		socket_.async_read_some(boost::asio::buffer(incomingData_, 1024),
		                        boost::bind(&TcpConnection::handleRead, shared_from_this(),
		                                    boost::asio::placeholders::error,
		                                    boost::asio::placeholders::bytes_transferred));
	}

	void writeMessage()
	{
		outgoingMessage_ = makeDaytimeString();

		boost::asio::async_write(socket_, boost::asio::buffer(outgoingMessage_),
		                         boost::bind(&TcpConnection::handleWrite, shared_from_this(),
		                                     boost::asio::placeholders::error,
		                                     boost::asio::placeholders::bytes_transferred));
	}

	void handleWrite(const boost::system::error_code & /*error*/, size_t /*bytes_transferred*/) {}
	void handleRead(const boost::system::error_code &error, size_t bytesTransferred)
	{
		incomingMessage_ = incomingData_;
		std::cout << "Got message: " << incomingMessage_ << std::endl;
		std::cout << "Got error: " << error.value() << std::endl;
		if (strcmp(incomingData_, "die") == 0)
		{
			std::cout << "Really dying now" << std::endl;
			running = false;
		}
		if (incomingMessage_ == "howdy")
		{
			writeMessage();
		}
		if (incomingMessage_ != "quit")
		{
			startRead();
		}
		if (incomingMessage_ == "die")
		{
			running = false;
		}
		if (error.value() == 2)
		{
			std::cout << "Connection severed, dying" << std::endl;
			running = false;
		}
	};

	tcp::socket socket_;
	std::string outgoingMessage_;
	char incomingData_[1024];
	std::string incomingMessage_;
};

class TcpServer
{
public:
	TcpServer(boost::asio::io_context &io_context)
	    : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 23456))
	{
		startAccept();
	}

	void runIoContext()
	{ // start the io context in a new thread
		try
		{
			std::cout << "Running io context..." << std::endl;
			io_context_.run();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
		std::cout << "Stopping..." << std::endl;
	}

private:
	void startAccept()
	{ // start accepting incoming tcp connections

		// first create a new connection and get a shared pointer to it
		// uses this object's io context
		TcpConnection::pointer newConnection = TcpConnection::create(io_context_);

		// use the new connection's socket to start accepting incoming connections
		// an incoming connection will
		acceptor_.async_accept(newConnection->socket(), boost::bind(&TcpServer::handleAccept, this, newConnection,
		                                                            boost::asio::placeholders::error));
	}

	void handleAccept(TcpConnection::pointer new_connection, const boost::system::error_code &error)
	{ // handle incoming connections
		if (!error)
		{
			new_connection->start();
		}

		startAccept();
	}

	boost::asio::io_context &io_context_;
	tcp::acceptor acceptor_;
};

int main()
{
	boost::asio::io_context io_context;
	TcpServer tcpServer(io_context);
	boost::thread t1(&TcpServer::runIoContext, &tcpServer);

	while (running)
	{
		usleep(1000);
	}

	std::cout << "Not running!" << std::endl;
	io_context.stop();
	t1.join();
	// try
	// {
	// 	boost::asio::io_context io_context;
	// 	TcpServer tcpServer(io_context);
	// 	io_context.run();
	// 	std::cout << "Done running" << std::endl;
	// }
	// catch (std::exception &e)
	// {
	// 	std::cerr << e.what() << std::endl;
	// }

	return 0;
}

#include "TcpServer.h"
#include "TcpConnection.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

TcpServer::TcpServer(std::function<ICommsApp *()> getNewCommsApp, boost::shared_ptr<boost::asio::io_context> ioContext,
                     boost::asio::ip::address ipAddress, unsigned short port)
    : getNewCommsApp_{getNewCommsApp}, ioContext_{ioContext}
{
	if (ioContext_.get() == nullptr)
	{
		ioContext_ = boost::make_shared<boost::asio::io_context>();
		ioContextCreatedByUs_ = true;
	}
	try
	{
		tcpAcceptor_ = boost::make_shared<boost::asio::ip::tcp::acceptor>(
		    *ioContext, boost::asio::ip::tcp::endpoint(ipAddress, port));
	}
	catch (boost::system::system_error &e)
	{
		std::cerr << e.what() << std::endl;
		valid_ = false;
	}
}

TcpServer::TcpServer(std::function<ICommsApp *()> getNewCommsApp, boost::shared_ptr<boost::asio::io_context> ioContext,
                     std::string pathName)
    : getNewCommsApp_{getNewCommsApp}, ioContext_{ioContext}
{
	if (ioContext_.get() == nullptr)
	{
		ioContext_ = boost::make_shared<boost::asio::io_context>();
		ioContextCreatedByUs_ = true;
	}
	try
	{
        ::unlink(pathName.c_str());
        boost::asio::local::stream_protocol::endpoint endpoint(pathName);
		udsAcceptor_ = boost::make_shared<boost::asio::local::stream_protocol::acceptor>(
		    *ioContext, endpoint);
	}
	catch (boost::system::system_error &e)
	{
		std::cerr << e.what() << std::endl;
		valid_ = false;
	}
}

TcpServer::~TcpServer()
{ // make sure the ioContext and its associated thread are stopped
	stop();
}


void TcpServer::run()
{
	if (!valid_)
	{ // only run if we are valid
		return;
	}

	// start the io_context, if created by us, otherwise assume it's run outside this instance
	if (ioContextCreatedByUs_)
	{
		ioContextThread_ = new boost::thread([this]() { ioContext_->run(); });
	}
	startAccept();
}

void TcpServer::stop()
{
	if (ioContextThread_ != nullptr)
	{ // we started the context, so we must end it
		ioContext_->stop();
		ioContextThread_->join();
		delete ioContextThread_;
		ioContextThread_ = nullptr;
	}
}

void TcpServer::startAccept()
{ //
	boost::shared_ptr<ICommsApp> commsApp = boost::make_shared<ICommsApp>(getNewCommsApp_());
	boost::shared_ptr<TcpConnection> newConnection(new TcpConnection(*ioContext_));

	// dynamic cast the shared pointer to newConnection (instance of TcpConnection)
	// to a shared pointer to its parent (ICommsConnection)
	commsApp->addConnection(boost::dynamic_pointer_cast<ICommsConnection>(newConnection));

	if (tcpAcceptor_.get() != nullptr)
	{
		tcpAcceptor_->async_accept(newConnection->socket(), boost::bind(&TcpServer::handleAccept, this, newConnection,
		                                                               boost::asio::placeholders::error));
	}
	else if (udsAcceptor_.get() != nullptr)
	{
		udsAcceptor_->async_accept(newConnection->socket(), boost::bind(&TcpServer::handleAccept, this, newConnection,
		                                                               boost::asio::placeholders::error));
	}
}

void TcpServer::handleAccept(boost::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error)
{
	if (!error)
	{
		newConnection->start();
	}

	startAccept();
}

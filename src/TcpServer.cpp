#include "TcpServer.h"
#include "TcpConnection.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

TcpServer::TcpServer(std::function<ICommsApp *()> getNewCommsApp, boost::shared_ptr<boost::asio::io_context> ioContext,
                     boost::asio::ip::address ipAddress, unsigned short port)
    : getNewCommsApp_{getNewCommsApp}, ioContext_{ioContext} tcpAcceptor_(
                                           ioContext, boost::asio::ip::tcp::endpoint(ipAddress, port))
{
	if (ioContext_.get() == nullptr)
	{
		ioContext_ = boost::make_shared<boost::asio::io_context>();
		ioContextCreatedByUs_ = true;
	}
}

TcpServer::~TcpServer()
{ // make sure the ioContext and its associated thread are stopped
	stop();
}


void TcpServer::run()
{ // start the io_context, if created by us, otherwise assume it's run outside this instance
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

	newConnection->socket().async_accept(
	    newConnection->socket(),
	    boost::bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
}

void TcpServer::handleAccept(boost::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error)
{
	if (!error)
	{
		newConnection->start();
	}

	startAccept();
}

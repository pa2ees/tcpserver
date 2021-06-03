#include "TcpServer.h"
#include "TcpConnection.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

TcpServer::TcpServer(std::function<ICommsApp *()> getNewCommsApp, boost::asio::ip::address ipAddress,
                     unsigned short port)
    : getNewCommsApp_{getNewCommsApp}
{
	ioContext_ = boost::make_shared<boost::asio::io_context>();
	ioContextCreatedByUs_ = true;

	try
	{
		tcpAcceptor_ = boost::make_shared<boost::asio::ip::tcp::acceptor>(
		    *ioContext_, boost::asio::ip::tcp::endpoint(ipAddress, port));
	}
	catch (boost::system::system_error &e)
	{
		std::cerr << e.what() << std::endl;
		valid_ = false;
	}
}


TcpServer::TcpServer(std::function<ICommsApp *()> getNewCommsApp, boost::shared_ptr<boost::asio::io_context> ioContext,
                     boost::asio::ip::address ipAddress, unsigned short port)
    : getNewCommsApp_{getNewCommsApp}, ioContext_{ioContext}
{
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

TcpServer::TcpServer(std::function<ICommsApp *()> getNewCommsApp, std::string pathName)
    : getNewCommsApp_{getNewCommsApp}
{
	ioContext_ = boost::make_shared<boost::asio::io_context>();
	ioContextCreatedByUs_ = true;

	try
	{
		::unlink(pathName.c_str());
		boost::asio::local::stream_protocol::endpoint endpoint(pathName);
		udsAcceptor_ = boost::make_shared<boost::asio::local::stream_protocol::acceptor>(*ioContext_, endpoint);
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
	try
	{
		::unlink(pathName.c_str());
		boost::asio::local::stream_protocol::endpoint endpoint(pathName);
		udsAcceptor_ = boost::make_shared<boost::asio::local::stream_protocol::acceptor>(*ioContext, endpoint);
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
		std::cout << "Running ioContext in another thread" << std::endl;
		ioContextThread_ = new boost::thread(&TcpServer::ioContextRun, this);
	}
	startAccept();
}

void TcpServer::ioContextRun()
{
	// this is a work guard, preventing ioContext_ from stopping if there is no work to be done
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard(ioContext_->get_executor());
	ioContext_->run();
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
{
	std::cout << "StartAccept..." << std::endl;

	std::cout << "Creating commsApp" << std::endl;
	boost::shared_ptr<ICommsApp> commsApp(getNewCommsApp_());

	std::cout << "Creating new TCP Connection" << std::endl;
	boost::shared_ptr<TcpConnection> newConnection(new TcpConnection(*ioContext_, commsApp));

	std::cout << "Adding connection to commsApp" << std::endl;
	// dynamic cast the shared pointer to newConnection (instance of TcpConnection)
	// to a shared pointer to its parent (ICommsConnection)
	commsApp->addConnection(boost::dynamic_pointer_cast<ICommsConnection>(newConnection));

	if (tcpAcceptor_)
	{
		std::cout << "Using tcpAcceptor to accept incoming connections" << std::endl;
		tcpAcceptor_->async_accept(newConnection->socket(), boost::bind(&TcpServer::handleAccept, this, newConnection,
		                                                                boost::asio::placeholders::error));
	}
	else if (udsAcceptor_)
	{
		std::cout << "Using udsAcceptor to accept incoming connections" << std::endl;
		udsAcceptor_->async_accept(newConnection->socket(), boost::bind(&TcpServer::handleAccept, this, newConnection,
		                                                                boost::asio::placeholders::error));
	}
}

void TcpServer::handleAccept(boost::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error)
{
	std::cout << "Handling accept" << std::endl;
	if (!error)
	{
		std::cout << "No Errors, starting new connection" << std::endl;
		newConnection->start();
	}

	std::cout << "starting accept again" << std::endl;
	startAccept();
}

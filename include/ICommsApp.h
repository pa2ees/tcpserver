#pragma once
#include "ICommsConnection.h"
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>

class ICommsApp
{
public:
	~ICommsApp() {}


	virtual int handleRead(std::string incomingMessage, const boost::system::error_code &error,
	                       size_t bytesTransferred) = 0;
	virtual int handleWrite(const boost::system::error_code &error, size_t bytesTransferred) = 0;
	virtual void connectionClosed() = 0;
	virtual void addConnection(boost::shared_ptr<ICommsConnection> commsConnection) = 0;
};

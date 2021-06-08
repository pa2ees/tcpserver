#include "CliServer.h"
#include <iostream>


CliServer::CliServer() {}
CliServer::~CliServer() {}

int CliServer::handleRead(std::string incomingMessage, const boost::system::error_code &error,
                          size_t bytesTransferred)
{ // do stuff
	int retval = 0;
	std::cout << "Got: " << incomingMessage << std::endl;

	if (incomingMessage == "who\r\n")
	{
		sendMessage("I am me");
	}

	return retval;
}
int CliServer::handleWrite(const boost::system::error_code &error, size_t bytesTransferred)
{ // At the moment, this is not used
	std::cout << "Handle Write" << std::endl;
	return 0;
}
void CliServer::sendMessage(std::string outgoingMessage)
{
	if (commsConnection_)
	{
		commsConnection_->writeMessage(outgoingMessage);
	}
	else
	{
		std::cout << "Could not write message, no commsConnection!" << std::endl;
	}
}
void CliServer::connectionClosed()
{ // do stuff
	std::cout << "Connection closed" << std::endl;
	commsConnection_.reset();
}
void CliServer::addConnection(boost::shared_ptr<ICommsConnection> commsConnection)
{ // do stuff
	std::cout << "Add Connection" << std::endl;
	commsConnection_ = commsConnection;
}

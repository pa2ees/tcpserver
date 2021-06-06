#pragma once
#include <ICommsApp.h>
#include <ICommsConnection.h>
#include <string>
#include <boost/shared_ptr.hpp>

class CliServer : public ICommsApp
{
public:
	CliServer();
	~CliServer();

	int handleRead(std::string incomingMessage, const boost::system::error_code &error,
	               size_t bytesTransferred) override;
	int handleWrite(const boost::system::error_code &error, size_t bytesTransferred) override;
	void sendMessage(std::string outgoingMessage);
	void connectionClosed() override;
	void addConnection(boost::shared_ptr<ICommsConnection> commsConnection) override;

private:
	boost::shared_ptr<ICommsConnection> commsConnection_ = {};
	bool isClientConnected = false;
};


class CliPacket
{
public:
	CliPacket();
	~CliPacket();


    bool isValid();
    
private:
    bool valid = false;
    std::string command = "";
    uint16_t payloadLength = 0;
    std::string payload = "";
    
};

#include <ICommsApp.h>
#include <ICommsConnection.h>
#include <TcpServer.h>
#include <iostream>

class Cli : public ICommsApp
{
public:
	Cli() {}
	~Cli() {}

	int handleRead(std::string incomingMessage, const boost::system::error_code &error,
	               size_t bytesTransferred) override
	{ // do stuff
		int retval = 0;
		std::cout << "Got: " << incomingMessage << std::endl;

		if (incomingMessage == "who\r\n")
		{
			sendMessage("I am me");
		}

		return retval;
	}
	int handleWrite(const boost::system::error_code &error, size_t bytesTransferred) override
	{ // At the moment, this is not used
		std::cout << "Handle Write" << std::endl;
		return 0;
	}
	void sendMessage(std::string outgoingMessage)
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
	void connectionClosed() override
	{ // do stuff
		std::cout << "Connection closed" << std::endl;
		commsConnection_.reset();
	}
	void addConnection(boost::shared_ptr<ICommsConnection> commsConnection) override
	{ // do stuff
		std::cout << "Add Connection" << std::endl;
		commsConnection_ = commsConnection;
	}

private:
	boost::shared_ptr<ICommsConnection> commsConnection_ = {};
};

Cli *createNewCli()
{
	return new Cli();
}

int main(int argc, char *argv[])
{
	std::string ip = "0.0.0.0";
	boost::system::error_code errorCode;
	boost::asio::ip::address ipAddress = boost::asio::ip::make_address(ip, errorCode);
	if (errorCode.value() != 0)
	{
		std::cerr << "Problem creating ip address" << std::endl;
		return -1;
	}

	TcpServer tcpServer([]() { return new Cli(); }, //
	                    ipAddress,                  //
	                    23457);                     //

	std::cout << "Running tcp server" << std::endl;
	tcpServer.run();

	sleep(30);

	std::cout << "Stopping tcp server" << std::endl;
	tcpServer.stop();

	return 0;
}

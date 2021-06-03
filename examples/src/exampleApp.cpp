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

		return retval;
	}
	int handleWrite(const boost::system::error_code &error, size_t bytesTransferred) override
	{ // At the moment, this is not used
		std::cout << "Handle Write" << std::endl;
		return 0;
	}
	void connectionClosed() override
	{ // do stuff
		std::cout << "Connection closed" << std::endl;
	}
	void addConnection(boost::shared_ptr<ICommsConnection> commsConnection) override
	{ // do stuff
		std::cout << "Add Connection" << std::endl;
	}

private:
};

int main(int argc, char *argv[])
{
	TcpServer<Cli> tcpServer;

	std::cout << "Running tcp server" << std::endl;
	tcpServer.run();

	sleep(30);

	std::cout << "Stopping tcp server" << std::endl;
	tcpServer.stop();

	return 0;
}

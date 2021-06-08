#include "CliServer.h"
#include <TcpServer.h>
#include <boost/asio.hpp>
#include <unistd.h>

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

	TcpServer tcpServer([]() { return new CliServer(); }, //
	                    ipAddress,                  //
	                    23457);                     //

	std::cout << "Running tcp server" << std::endl;
	tcpServer.run();

	sleep(30);

	std::cout << "Stopping tcp server" << std::endl;
	tcpServer.stop();

	return 0;
}

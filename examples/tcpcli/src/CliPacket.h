#pragma once
#include "ICliPacket.h"
#include <string>

class CliPacket : public ICliPacket
{
public:
	CliPacket();
    CliPacket(ICliPacket::Command command, std::string payload);
    CliPacket(std::string commandString, std::string payload);
    CliPacket(std::string packet);

	~CliPacket();

    bool isValid() override;
    std::string toStr() override;
    
private:
    Command getValidCommandFromStr(std::string command);

    bool valid_ = false;
    Command command_ = NOP;
    uint16_t payloadLength_ = 0;
    std::string payload_ = "";
    
};

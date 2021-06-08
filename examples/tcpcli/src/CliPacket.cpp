#include "CliPacket.h"
#include <iostream>
#include <sstream>
#include <arpa/inet.h>

CliPacket::CliPacket() {}
CliPacket::CliPacket(std::string commandString, std::string payload)
{
    valid_ = true;
    command_ = getValidCommandFromStr(commandString);
    if (command_ == NOP)
    {
        valid_ = false;
        std::cerr << "Invalid command string" << std::endl;
    }
    payload_ = payload;
    payloadLength_ = htons(payload.size());
}

CliPacket::CliPacket(ICliPacket::Command command, std::string payload)
{
    valid_ = true;
    command_ = command;
    if (command_ == NOP)
    {
        valid_ = false;
        std::cerr << "Invalid command string" << std::endl;
    }
    payload_ = payload;
    payloadLength_ = htons(payload.size());
}

CliPacket::CliPacket(std::string packet)
{
    valid_ = true;
    if (packet.size() < 6)
    { // packets must be at least 6 bytes long
        valid_ = false;
    }
    else
    {
        command_ = getValidCommandFromStr(packet.substr(HEADER_OFFSET, LENGTH_OFFSET));
        if (command_ == NOP)
        {
            std::cerr << "Invalid command" << std::endl;
            valid_ = false;
        }

        // cast each char of the length field to uint8
        uint8_t byte0 = static_cast<uint8_t> (packet.c_str()[LENGTH_OFFSET]);
        uint8_t byte1 = static_cast<uint8_t> (packet.c_str()[LENGTH_OFFSET+1]);

        payloadLength_ = htons((byte0 << 8) | byte1);
        if (packet.size() - PAYLOAD_OFFSET != payloadLength_)
        { // invalid length field
            std::cerr << "Invalid length field. Payload is " << (packet.size() - PAYLOAD_OFFSET) <<
                " bytes long, but length field is " << payloadLength_ << std::endl;
            valid_ = false;
        }
        payload_ = packet.substr(PAYLOAD_OFFSET, PAYLOAD_OFFSET + payloadLength_);
    }            
}

CliPacket::~CliPacket() {}

bool CliPacket::isValid()
{
    return valid_;
}

ICliPacket::Command CliPacket::getValidCommandFromStr(std::string command)
{
    if (command == "cmp")
    {
        return CMP;
    }
    if (command == "cmd")
    {
        return CMD;
    }
    return NOP;
}

std::string CliPacket::toStr()
{
    std::stringstream retval;
    if (isValid())
    {
        retval << commandStrings[command_];
        retval << static_cast<uint8_t>((htons(payloadLength_) >> 8) & 0xFF);
        retval << static_cast<uint8_t>((htons(payloadLength_)) & 0xFF);
        retval << payload_;
    }
    else
    {
        retval << "";
        std::cerr << "Attempting to convert invalid packet to string" << std::endl;
    }
    
    return retval.str();
}

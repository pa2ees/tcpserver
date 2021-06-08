#pragma once
#include <string>

class ICliPacket
{
public:

    enum Command
    {
        NOP,
        CMP,
        CMD,
    };
    const char *const commandStrings[3] = {"nop", "cmp", "cmd"};

    virtual bool isValid() = 0;
    virtual std::string toStr() = 0;

protected:
    static const int HEADER_OFFSET = 0;
    static const int LENGTH_OFFSET = 3;
    static const int PAYLOAD_OFFSET = 5;


};

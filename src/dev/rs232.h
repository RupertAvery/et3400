#ifndef RS232_H
#define RS232_H
#include <QDebug>
#include "rs232.h"
#include "../common/common_defs.h"

class RS232Adapter
{
private:
    int sendState = 0;
    int sendBuffer = 0;
    int rcvState = 0;
    uint8_t rcvBuffer = 0x7F;
    //Queue<int> inputBuffer = new Queue<int>();
    int tempBuffer;

public:
    virtual void receiveByte(uint8_t value);
    void receiveString(char *value);
    uint8_t receive();
    void send(uint8_t value);
};


class DebugConsoleAdapter : public RS232Adapter
{
public:
    void receiveByte(uint8_t value) override;
};

#endif // RS232_H

#include "rs232.h"
#include "stdio.h"

void DebugConsoleAdapter::receiveByte(uint8_t value)
{
    printf("%c", value);
}


void RS232Adapter::receiveByte(uint8_t value){
    // foreach (var chr in value)
    // {
    //     inputBuffer.Enqueue(chr);
    // }
};

void RS232Adapter::receiveString(char *value){
    // foreach (var chr in value)
    // {
    //     inputBuffer.Enqueue(chr);
    // }
};

uint8_t RS232Adapter::receive()
{
    // The PIA is wired like so for Peripheral A
    // PA0 - Output bit (pulled high)
    // PA1 - pulled high w/ jumper to ground
    // PA2 - pulled high w/ jumper to ground
    // PA3 - pulled high w/ jumper to ground
    // PA4 - pulled low
    // PA5 - NC
    // PA6 - NC
    // PA7 - Input bit

    // Y--0111X
    // Y is the input. 
    // Mask: 10000000 = 0x80
    // Mask: 00001110 = 0x0E
    // Mask: 11101111 = 0xEF



    uint8_t value = 0x7E;

    if (rcvState == 0)
    {
         if (!inputBuffer->empty() > 0)
         {
             tempBuffer = inputBuffer->front();
             inputBuffer->pop();
             rcvState++;
             value = 0xFF;
         }
    }
    else if (rcvState == 1)
    {
        value = 0B00001110;
        rcvState++;
    }
    //else if (rcvState == 2)
    //{
    //    value = 0x7F;
    //    rcvState++;
    //}
    else if (rcvState == 10)
    {
        value = 0x7F;
        rcvState = 0;
    }
    else if (rcvState == 11)
    {
        value = 0x7F;
        rcvState = 0;
    }
    else
    {
        //value = 0x7E | (tempBuffer >> rcvState - 1);
        value = tempBuffer;
        value = value << (7 - rcvState + 2);
        value = value & 0x80; // mask all bits but the 8th
        value = value | 0x0E; // set bits 1-3
        value = value & 0xEF; // clear bit 4
        rcvState++;
    }

    return value;
}

// int RS232Adapter::WriteEx()
// {
//     var value = 0x7F;

//     if (rcvState == 0)
//     {
//         if (inputBuffer.Count > 0)
//         {
//             tempBuffer = inputBuffer.Dequeue();
//             rcvState++;
//             value = 0xFF;
//         }
//     }
//     else if (rcvState == 1)
//     {
//         value = 0xFF;
//         rcvState++;
//     }
//     else if (rcvState == 2)
//     {
//         value = 0x7F;
//         rcvState++;
//     }
//     else if (rcvState == 11)
//     {
//         rcvState = 0;
//         value = 0x7F;
//     }
//     else
//     {
//         //value = 0x7E | (tempBuffer >> rcvState - 1);
//         value = tempBuffer << 7 - (rcvState - 3);
//         value = ~value & 0xFF;
//         //value = value & 0x80;
//         //value = value | 0b1110;
//         rcvState++;
//     }

//     return value;
// }

void RS232Adapter::send(uint8_t value)
{
    //value &= 1;

    if (sendState == 0)
    {
        if (value == 0)
        {
            sendBuffer = 0;
            sendState++;
        }
    }
    else if (sendState == 9)
    {
        if (value == 1)
        {
            if (sendBuffer > 0)
            {
                receiveByte(sendBuffer);
            }
            sendState = 0;
        }
    }
    else
    {
        sendBuffer |= (value << (sendState - 1));
        sendState++;
    }
}

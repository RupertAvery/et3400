#include "mc6820.h"

MC6820::MC6820(RS232Adapter *rs232adapter)
{
    _rs232adapter = rs232adapter;
}

uint8_t MC6820::read(offs_t addr)
{
    return get(addr & 3);
};

void MC6820::write(offs_t addr, uint8_t data)
{
    set(addr & 3, data);
};

bool MC6820::is_mapped(offs_t addr)
{
    return addr >= 0x1000 && addr <= 0x1006;
}

uint8_t *MC6820::get_mapped_memory()
{
    return nullptr;
}

offs_t MC6820::get_start()
{
    return 0x1000;
}

offs_t MC6820::get_end()
{
    return 0x1006;
}

void MC6820::set(int registerSelect, uint8_t value)
{
    switch (registerSelect)
    {
    // RS1 = 0, RS0 = 0
    case 0:
        switch (CRA & 4)
        {
        // CRA-B4 = 1
        case 4:
            PRA = value;
            _rs232adapter->send(value & 1);
            //OnPeripheralWrite ?.Invoke(this, new PeripheralEventArgs(Peripheral.PRA, value));
            return;
        // CRA-B4 = 0
        case 0:
            DDRA = value;
            return;
        }
        break;
    // RS1 = 0, RS0 = 1
    case 1:
        CRA = value;
        return;
    // RS1 = 1, RS0 = 0
    case 2:
        switch (CRB & 4)
        {
        // CRB-B4 = 1
        case 4:
            PRB = value;
            _rs232adapter->send(value & 1);
            //OnPeripheralWrite ?.Invoke(this, new PeripheralEve    ntArgs(Peripheral.PRB, value));
            return;
        // CRB-B4 = 0
        case 0:
            DDRB = value;
            return;
        }
        break;
    // RS1 = 1, RS0 = 1
    case 3:
        CRB = value;
        return;
    }
    //throw new Exception("Invalid state");
}

uint8_t MC6820::get(int registerSelect)
{
    switch (registerSelect)
    {
    // RS1 = 0, RS0 = 0
    case 0:
        switch (CRA & 4)
        {
        // CRA-B4 = 1
        case 4:
            // var eventArgs = new PeripheralEventArgs(Peripheral.PRA);
            // OnPeripheralRead.Invoke(this, eventArgs);
            return _rs232adapter->receive(); //eventArgs.Value;
        // CRA-B4 = 0
        case 0:
            return DDRA;
        }
        break;
    // RS1 = 0, RS0 = 1
    case 1:
        return CRA;
    // RS1 = 1, RS0 = 0
    case 2:
        switch (CRB & 4)
        {
        // CRB-B4 = 1
        case 4:
            // var eventArgs = new PeripheralEventArgs(Peripheral.PRB);
            // OnPeripheralRead.Invoke(this, eventArgs);
            return _rs232adapter->receive(); //eventArgs.Value;
        // CRB-B4 = 0
        case 0:
            return DDRB;
        }
        break;
    // RS1 = 0, RS0 = 1
    case 3:
        return CRB;
    }
    //throw new Exception("Invalid state");
}

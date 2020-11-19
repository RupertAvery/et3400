#ifndef MC6820_H
#define MC6820_H

#include <QDebug>
#include "../common/common_defs.h"
#include "memory_mapped_device.h"
#include "rs232.h"

// enum Peripheral
// {
//     PRA,
//     PRB
// };

enum Register
{
    PRA,
    DDRA,
    CRA,
    PRB,
    DDRB,
    CRB,
};

class MC6820 : public memory_mapped_device
{
    /**
         * RS0/1 = Register Select 0/1
         * CRA/B = Control Register A/B
         * DDRA/B = Data Direction Register A/B
         * PRA/B = Peripheral Register A/B
         */

private:
    int CRA = 0;
    int CRB = 0;
    int DDRA = 0;
    int DDRB = 0;
    int PRA = 0;
    int PRB = 0;
    //Register register = Register::DDRA;

public:
    MC6820(RS232Adapter *rs232adapter);
    // ~display_io();
    uint8_t read(offs_t addr) override;
    void write(offs_t addr, uint8_t data) override;
    bool is_mapped(offs_t addr) override;
    uint8_t *get_mapped_memory() override;
    offs_t get_start() override;
    offs_t get_end() override;

private:
    RS232Adapter *_rs232adapter;
    void set(int registerSelect, uint8_t value);
    uint8_t get(int registerSelect);
};

#endif // MC6820_H

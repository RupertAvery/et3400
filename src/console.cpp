#include "console.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <stdio.h>

using namespace std;

void start_console()
{
    string inFileName = "monitor.bin";
    ifstream inFile;
    inFile.open(inFileName.c_str());

    int rom_size = 0x400;
    char *buffer;

    if (inFile.is_open())
    {
        buffer = (char *)malloc(rom_size);

        inFile.read(buffer, rom_size);

        inFile.close(); // CLose input file
    }
    else
    { //Error message
        cerr << "Can't find input file " << inFileName << endl;
    }

    emu = new et3400emu;

    emu->loadROM(0xFC00, (uint8_t *)buffer, rom_size);

    emu->init();
    emu->start();

    // device->device_start();
    // device->device_reset();

    cout << "Exiting.." << endl;

    free(emu);
}

void render_memory(et3400emu *emu)
{
    char *sbuffer = (char *)malloc(1024);

    while (emu->get_running())
    {
        gotoxy(0, 0);
        int baseaddr = 0;
        int ptr = 0;

        for (int y = 0; y < 16; y++)
        {
            sprintf(&sbuffer[ptr], "%04X: ", baseaddr + y * 16);
            ptr += 6;
            for (int x = 0; x < 16; x++)
            {
                sprintf(&sbuffer[ptr], "%02X ", emu->get_memory()[baseaddr + y * 16 + x]);
                ptr += 3;
            }
            sprintf(&sbuffer[ptr], "\n");
            ptr += 1;
        }
        printf("%s", sbuffer);
    }
}
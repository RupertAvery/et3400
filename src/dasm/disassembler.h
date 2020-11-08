#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h> 

struct Disassembly
{
    char *operand;
    char *arguments;
    int length;
    uint8_t *bytes;
};

struct DasmResult
{
    bool is_illegal;
    char *operand;
    char *instruction;
    int flags;
    int byteLength;
};

class Disassembler
{

    ///* some macros to keep things short */
    //#define OP      oprom[0]
    //#define ARG1    opram[1]
    //#define ARG2    opram[2]
    //#define ARGW    (opram[1]<<8) + opram[2]
    static const int DASMFLAG_SUPPORTED = 0x04;
    static const int DASMFLAG_STEP_OVER = 0x08;
    static const int DASMFLAG_STEP_OUT = 0x10;

    static const int inh = 1;  /* inherent */
    static const int rel = 2;  /* relative */
    static const int imb = 3;  /* immediate (byte) */
    static const int imw = 4;  /* immediate (word) */
    static const int dir = 5;  /* direct address */
    static const int imd = 6;  /* HD63701YO: immediate, direct address */
    static const int ext = 7;  /* extended address */
    static const int idx = 8;  /* x + byte offset */
    static const int imx = 9;  /* HD63701YO: immediate, x + byte offset */
    static const int sx1 = 10; /* HD63701YO: undocumented opcodes: byte from (s+1) */

    static const int aba = 0;
    static const int abx = 1;
    static const int adca = 2;
    static const int adcb = 3;
    static const int adda = 4;
    static const int addb = 5;
    static const int addd = 6;
    static const int aim = 7;
    static const int anda = 8;
    static const int andb = 9;
    static const int asl = 10;
    static const int asla = 11;
    static const int aslb = 12;
    static const int asld = 13;
    static const int asr = 14;
    static const int asra = 15;
    static const int asrb = 16;
    static const int bcc = 17;
    static const int bcs = 18;
    static const int beq = 19;
    static const int bge = 20;
    static const int bgt = 21;
    static const int bhi = 22;
    static const int bita = 23;
    static const int bitb = 24;
    static const int ble = 25;
    static const int bls = 26;
    static const int blt = 27;
    static const int bmi = 28;
    static const int bne = 29;
    static const int bpl = 30;
    static const int bra = 31;
    static const int brn = 32;
    static const int bsr = 33;
    static const int bvc = 34;
    static const int bvs = 35;
    static const int cba = 36;
    static const int clc = 37;
    static const int cli = 38;
    static const int clr = 39;
    static const int clra = 40;
    static const int clrb = 41;
    static const int clv = 42;
    static const int cmpa = 43;
    static const int cmpb = 44;
    static const int cmpx = 45;
    static const int com = 46;
    static const int coma = 47;
    static const int comb = 48;
    static const int daa = 49;
    static const int dec = 50;
    static const int deca = 51;
    static const int decb = 52;
    static const int des = 53;
    static const int dex = 54;
    static const int eim = 55;
    static const int eora = 56;
    static const int eorb = 57;
    static const int ill = 58;
    static const int inc = 59;
    static const int inca = 60;
    static const int incb = 61;
    static const int ins = 62;
    static const int inx = 63;
    static const int jmp = 64;
    static const int jsr = 65;
    static const int lda = 66;
    static const int ldb = 67;
    static const int ldd = 68;
    static const int lds = 69;
    static const int ldx = 70;
    static const int lsr = 71;
    static const int lsra = 72;
    static const int lsrb = 73;
    static const int lsrd = 74;
    static const int mul = 75;
    static const int neg = 76;
    static const int nega = 77;
    static const int negb = 78;
    static const int nop = 79;
    static const int oim = 80;
    static const int ora = 81;
    static const int orb = 82;
    static const int psha = 83;
    static const int pshb = 84;
    static const int pshx = 85;
    static const int pula = 86;
    static const int pulb = 87;
    static const int pulx = 88;
    static const int rol = 89;
    static const int rola = 90;
    static const int rolb = 91;
    static const int ror = 92;
    static const int rora = 93;
    static const int rorb = 94;
    static const int rti = 95;
    static const int rts = 96;
    static const int sba = 97;
    static const int sbca = 98;
    static const int sbcb = 99;
    static const int sec = 100;
    static const int sev = 101;
    static const int sta = 102;
    static const int stb = 103;
    static const int _std = 104;
    static const int sei = 105;
    static const int sts = 106;
    static const int stx = 107;
    static const int suba = 108;
    static const int subb = 109;
    static const int subd = 110;
    static const int swi = 111;
    static const int wai = 112;
    static const int tab = 113;
    static const int tap = 114;
    static const int tba = 115;
    static const int tim = 116;
    static const int tpa = 117;
    static const int tst = 118;
    static const int tsta = 119;
    static const int tstb = 120;
    static const int tsx = 121;
    static const int txs = 122;
    static const int asx1 = 123;
    static const int asx2 = 124;
    static const int xgdx = 125;
    static const int addx = 126;
    static const int adcx = 127;
    static const int nba = 128;

    static const char op_name_str_orig[128][8];
    static const char op_name_str[129][8];
    static int table[258][3];

    // public
    //     bool EnableUndocumentedOpcodes
    //     {
    //         get { return _enableUndocumentedOpcodes; }
    //         set
    //         {
    //             _enableUndocumentedOpcodes = value;
    //             var patchValue = _enableUndocumentedOpcodes ? 0 : 1;
    //             table[0x14][2] = patchValue;
    //             table[0x87][2] = patchValue;
    //             table[0x8f][2] = patchValue;
    //             table[0xc7][2] = patchValue;
    //             table[0xcF][2] = patchValue;
    //         }
    //     };

    static int SIGNED(int b);
    static bool IsSubroutine(int opcode);
    static bool IsReturn(int opcode);

public:
    static DasmResult disassemble(uint8_t *memory, int address);
    //     static int Disassemble(int[] memory, int pc, ref string buf);
    //     static void SelfTest();

    // List of 197 valid opcodes
    static constexpr int valid6800opcodes[197] =
        {
            0x01,
            0x06,
            0x07,
            0x08,
            0x09,
            0x0A,
            0x0B,
            0x0C,
            0x0D,
            0x0E,
            0x0F,
            0x10,
            0x11,
            0x16,
            0x17,
            0x19,
            0x1B,
            0x20,
            0x22,
            0x23,
            0x24,
            0x25,
            0x26,
            0x27,
            0x28,
            0x29,
            0x2A,
            0x2B,
            0x2C,
            0x2D,
            0x2E,
            0x2F,
            0x30,
            0x31,
            0x32,
            0x33,
            0x34,
            0x35,
            0x36,
            0x37,
            0x39,
            0x3B,
            0x3E,
            0x3F,
            0x40,
            0x43,
            0x44,
            0x46,
            0x47,
            0x48,
            0x49,
            0x4A,
            0x4C,
            0x4D,
            0x4F,
            0x50,
            0x53,
            0x54,
            0x56,
            0x57,
            0x58,
            0x59,
            0x5A,
            0x5C,
            0x5D,
            0x5F,
            0x60,
            0x63,
            0x64,
            0x66,
            0x67,
            0x68,
            0x69,
            0x6A,
            0x6C,
            0x6D,
            0x6E,
            0x6F,
            0x70,
            0x73,
            0x74,
            0x76,
            0x77,
            0x78,
            0x79,
            0x7A,
            0x7C,
            0x7D,
            0x7E,
            0x7F,
            0x80,
            0x81,
            0x82,
            0x84,
            0x85,
            0x86,
            0x88,
            0x89,
            0x8A,
            0x8B,
            0x8C,
            0x8D,
            0x8E,
            0x90,
            0x91,
            0x92,
            0x94,
            0x95,
            0x96,
            0x97,
            0x98,
            0x99,
            0x9A,
            0x9B,
            0x9C,
            0x9E,
            0x9F,
            0xA0,
            0xA1,
            0xA2,
            0xA4,
            0xA5,
            0xA6,
            0xA7,
            0xA8,
            0xA9,
            0xAA,
            0xAB,
            0xAC,
            0xAD,
            0xAE,
            0xAF,
            0xB0,
            0xB1,
            0xB2,
            0xB4,
            0xB5,
            0xB6,
            0xB7,
            0xB8,
            0xB9,
            0xBA,
            0xBB,
            0xBC,
            0xBD,
            0xBE,
            0xBF,
            0xC0,
            0xC1,
            0xC2,
            0xC4,
            0xC5,
            0xC6,
            0xC8,
            0xC9,
            0xCA,
            0xCB,
            0xCE,
            0xD0,
            0xD1,
            0xD2,
            0xD4,
            0xD5,
            0xD6,
            0xD7,
            0xD8,
            0xD9,
            0xDA,
            0xDB,
            0xDE,
            0xDF,
            0xE0,
            0xE1,
            0xE2,
            0xE4,
            0xE5,
            0xE6,
            0xE7,
            0xE8,
            0xE9,
            0xEA,
            0xEB,
            0xEE,
            0xEF,
            0xF0,
            0xF1,
            0xF2,
            0xF4,
            0xF5,
            0xF6,
            0xF7,
            0xF8,
            0xF9,
            0xFA,
            0xFB,
            0xFE,
            0xFF};

    // private
    //     bool _enableUndocumentedOpcodes;
};

#endif //DISASSEMBLER_H
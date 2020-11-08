#include "disassembler.h"


const char Disassembler::op_name_str_orig[128][8] =
    {
        "aba", "abx", "adca", "adcb", "adda", "addb", "addd", "aim",
        "anda", "andb", "asl", "asla", "aslb", "asld", "asr", "asra",
        "asrb", "bcc", "bcs", "beq", "bge", "bgt", "bhi", "bita",
        "bitb", "ble", "bls", "blt", "bmi", "bne", "bpl", "bra",
        "brn", "bsr", "bvc", "bvs", "cba", "clc", "cli", "clr",
        "clra", "clrb", "clv", "cmpa", "cmpb", "cmpx", "com", "coma",
        "comb", "daa", "dec", "deca", "decb", "des", "dex", "eim",
        "eora", "eorb", "illegal", "inc", "inca", "incb", "ins", "inx",
        "jmp", "jsr", "lda", "ldb", "ldd", "lds", "ldx", "lsr",
        "lsra", "lsrb", "lsrd", "mul", "neg", "nega", "negb", "nop",
        "oim", "ora", "orb", "psha", "pshb", "pshx", "pula", "pulb",
        "pulx", "rol", "rola", "rolb", "ror", "rora", "rorb", "rti",
        "rts", "sba", "sbca", "sbcb", "sec", "sev", "sta", "stb",
        "std", "sei", "sts", "stx", "suba", "subb", "subd", "swi",
        "wai", "tab", "tap", "tba", "tim", "tpa", "tst", "tsta",
        "tstb", "tsx", "txs", "asx1", "asx2", "xgdx", "addx", "adcx"};

const char Disassembler::op_name_str[129][8] =
    {
        "aba", "abx", "adca", "adcb", "adda", "addb", "addd", "aim",
        "anda", "andb", "asl", "asla", "aslb", "asld", "asr", "asra",
        "asrb", "bcc", "bcs", "beq", "bge", "bgt", "bhi", "bita",
        "bitb", "ble", "bls", "blt", "bmi", "bne", "bpl", "bra",
        "brn", "bsr", "bvc", "bvs", "cba", "clc", "cli", "clr",
        "clra", "clrb", "clv", "cmpa", "cmpb", "cpx", "com", "coma",
        "comb", "daa", "dec", "deca", "decb", "des", "dex", "eim",
        "eora", "eorb", "illegal", "inc", "inca", "incb", "ins", "inx",
        "jmp", "jsr", "ldaa", "ldab", "ldd", "lds", "ldx", "lsr",
        "lsra", "lsrb", "lsrd", "mul", "neg", "nega", "negb", "nop",
        "oim", "oraa", "orab", "psha", "pshb", "pshx", "pula", "pulb",
        "pulx", "rol", "rola", "rolb", "ror", "rora", "rorb", "rti",
        "rts", "sba", "sbca", "sbcb", "sec", "sev", "staa", "stab",
        "std", "sei", "sts", "stx", "suba", "subb", "subd", "swi",
        "wai", "tab", "tap", "tba", "tim", "tpa", "tst", "tsta",
        "tstb", "tsx", "txs", "asx1", "asx2", "xgdx", "addx", "adcx",
        "nba"};

/*
 * This table defines the opcodes:
 * byte meaning
 * 0    token (menmonic)
 * 1    addressing mode
 * 2    invalid opcode for 1:6800/6802/6808, 2:6801/6803, 4:HD63701
 */

int Disassembler::table[258][3] = {
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::nop, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    /* 00 */
    {Disassembler::lsrd, Disassembler::inh, 1},
    {Disassembler::asld, Disassembler::inh, 1},
    {Disassembler::tap, Disassembler::inh, 0},
    {Disassembler::tpa, Disassembler::inh, 0},
    {Disassembler::inx, Disassembler::inh, 0},
    {Disassembler::dex, Disassembler::inh, 0},
    {Disassembler::clv, Disassembler::inh, 0},
    {Disassembler::sev, Disassembler::inh, 0},
    {Disassembler::clc, Disassembler::inh, 0},
    {Disassembler::sec, Disassembler::inh, 0},
    {Disassembler::cli, Disassembler::inh, 0},
    {Disassembler::sei, Disassembler::inh, 0},
    {Disassembler::sba, Disassembler::inh, 0},
    {Disassembler::cba, Disassembler::inh, 0},
    {Disassembler::asx1, Disassembler::sx1, 1},
    {Disassembler::asx2, Disassembler::sx1, 1},
    /* 10 */
    {Disassembler::nba, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::tab, Disassembler::inh, 0},
    {Disassembler::tba, Disassembler::inh, 0},
    {Disassembler::xgdx, Disassembler::inh, 3},
    {Disassembler::daa, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::aba, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::bra, Disassembler::rel, 0},
    {Disassembler::brn, Disassembler::rel, 1},
    {Disassembler::bhi, Disassembler::rel, 0},
    {Disassembler::bls, Disassembler::rel, 0},
    /* 20 */
    {Disassembler::bcc, Disassembler::rel, 0},
    {Disassembler::bcs, Disassembler::rel, 0},
    {Disassembler::bne, Disassembler::rel, 0},
    {Disassembler::beq, Disassembler::rel, 0},
    {Disassembler::bvc, Disassembler::rel, 0},
    {Disassembler::bvs, Disassembler::rel, 0},
    {Disassembler::bpl, Disassembler::rel, 0},
    {Disassembler::bmi, Disassembler::rel, 0},
    {Disassembler::bge, Disassembler::rel, 0},
    {Disassembler::blt, Disassembler::rel, 0},
    {Disassembler::bgt, Disassembler::rel, 0},
    {Disassembler::ble, Disassembler::rel, 0},
    {Disassembler::tsx, Disassembler::inh, 0},
    {Disassembler::ins, Disassembler::inh, 0},
    {Disassembler::pula, Disassembler::inh, 0},
    {Disassembler::pulb, Disassembler::inh, 0},
    /* 30 */
    {Disassembler::des, Disassembler::inh, 0},
    {Disassembler::txs, Disassembler::inh, 0},
    {Disassembler::psha, Disassembler::inh, 0},
    {Disassembler::pshb, Disassembler::inh, 0},
    {Disassembler::pulx, Disassembler::inh, 1},
    {Disassembler::rts, Disassembler::inh, 0},
    {Disassembler::abx, Disassembler::inh, 1},
    {Disassembler::rti, Disassembler::inh, 0},
    {Disassembler::pshx, Disassembler::inh, 1},
    {Disassembler::mul, Disassembler::inh, 1},
    {Disassembler::wai, Disassembler::inh, 0},
    {Disassembler::swi, Disassembler::inh, 0},
    {Disassembler::nega, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::coma, Disassembler::inh, 0},
    /* 40 */
    {Disassembler::lsra, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::rora, Disassembler::inh, 0},
    {Disassembler::asra, Disassembler::inh, 0},
    {Disassembler::asla, Disassembler::inh, 0},
    {Disassembler::rola, Disassembler::inh, 0},
    {Disassembler::deca, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::inca, Disassembler::inh, 0},
    {Disassembler::tsta, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::clra, Disassembler::inh, 0},
    {Disassembler::negb, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::comb, Disassembler::inh, 0},
    /* 50 */
    {Disassembler::lsrb, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::rorb, Disassembler::inh, 0},
    {Disassembler::asrb, Disassembler::inh, 0},
    {Disassembler::aslb, Disassembler::inh, 0},
    {Disassembler::rolb, Disassembler::inh, 0},
    {Disassembler::decb, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::incb, Disassembler::inh, 0},
    {Disassembler::tstb, Disassembler::inh, 0},
    {Disassembler::ill, Disassembler::inh, 7},
    {Disassembler::clrb, Disassembler::inh, 0},
    {Disassembler::neg, Disassembler::idx, 0},
    {Disassembler::aim, Disassembler::imx, 3},
    {Disassembler::oim, Disassembler::imx, 3},
    {Disassembler::com, Disassembler::idx, 0},
    /* 60 */
    {Disassembler::lsr, Disassembler::idx, 0},
    {Disassembler::eim, Disassembler::imx, 3},
    {Disassembler::ror, Disassembler::idx, 0},
    {Disassembler::asr, Disassembler::idx, 0},
    {Disassembler::asl, Disassembler::idx, 0},
    {Disassembler::rol, Disassembler::idx, 0},
    {Disassembler::dec, Disassembler::idx, 0},
    {Disassembler::tim, Disassembler::imx, 3},
    {Disassembler::inc, Disassembler::idx, 0},
    {Disassembler::tst, Disassembler::idx, 0},
    {Disassembler::jmp, Disassembler::idx, 0},
    {Disassembler::clr, Disassembler::idx, 0},
    {Disassembler::neg, Disassembler::ext, 0},
    {Disassembler::aim, Disassembler::imd, 3},
    {Disassembler::oim, Disassembler::imd, 3},
    {Disassembler::com, Disassembler::ext, 0},
    /* 70 */
    {Disassembler::lsr, Disassembler::ext, 0},
    {Disassembler::eim, Disassembler::imd, 3},
    {Disassembler::ror, Disassembler::ext, 0},
    {Disassembler::asr, Disassembler::ext, 0},
    {Disassembler::asl, Disassembler::ext, 0},
    {Disassembler::rol, Disassembler::ext, 0},
    {Disassembler::dec, Disassembler::ext, 0},
    {Disassembler::tim, Disassembler::imd, 3},
    {Disassembler::inc, Disassembler::ext, 0},
    {Disassembler::tst, Disassembler::ext, 0},
    {Disassembler::jmp, Disassembler::ext, 0},
    {Disassembler::clr, Disassembler::ext, 0},
    {Disassembler::suba, Disassembler::imb, 0},
    {Disassembler::cmpa, Disassembler::imb, 0},
    {Disassembler::sbca, Disassembler::imb, 0},
    {Disassembler::subd, Disassembler::imw, 1},
    /* 80 */
    {Disassembler::anda, Disassembler::imb, 0},
    {Disassembler::bita, Disassembler::imb, 0},
    {Disassembler::lda, Disassembler::imb, 0},
    {Disassembler::sta, Disassembler::imb, 7},
    {Disassembler::eora, Disassembler::imb, 0},
    {Disassembler::adca, Disassembler::imb, 0},
    {Disassembler::ora, Disassembler::imb, 0},
    {Disassembler::adda, Disassembler::imb, 0},
    {Disassembler::cmpx, Disassembler::imw, 0},
    {Disassembler::bsr, Disassembler::rel, 0},
    {Disassembler::lds, Disassembler::imw, 0},
    {Disassembler::sts, Disassembler::imw, 7},
    {Disassembler::suba, Disassembler::dir, 0},
    {Disassembler::cmpa, Disassembler::dir, 0},
    {Disassembler::sbca, Disassembler::dir, 0},
    {Disassembler::subd, Disassembler::dir, 1},
    /* 90 */
    {Disassembler::anda, Disassembler::dir, 0},
    {Disassembler::bita, Disassembler::dir, 0},
    {Disassembler::lda, Disassembler::dir, 0},
    {Disassembler::sta, Disassembler::dir, 0},
    {Disassembler::eora, Disassembler::dir, 0},
    {Disassembler::adca, Disassembler::dir, 0},
    {Disassembler::ora, Disassembler::dir, 0},
    {Disassembler::adda, Disassembler::dir, 0},
    {Disassembler::cmpx, Disassembler::dir, 0},
    {Disassembler::jsr, Disassembler::dir, 1},
    {Disassembler::lds, Disassembler::dir, 0},
    {Disassembler::sts, Disassembler::dir, 0},
    {Disassembler::suba, Disassembler::idx, 0},
    {Disassembler::cmpa, Disassembler::idx, 0},
    {Disassembler::sbca, Disassembler::idx, 0},
    {Disassembler::subd, Disassembler::idx, 1},
    /* a0 */
    {Disassembler::anda, Disassembler::idx, 0},
    {Disassembler::bita, Disassembler::idx, 0},
    {Disassembler::lda, Disassembler::idx, 0},
    {Disassembler::sta, Disassembler::idx, 0},
    {Disassembler::eora, Disassembler::idx, 0},
    {Disassembler::adca, Disassembler::idx, 0},
    {Disassembler::ora, Disassembler::idx, 0},
    {Disassembler::adda, Disassembler::idx, 0},
    {Disassembler::cmpx, Disassembler::idx, 0},
    {Disassembler::jsr, Disassembler::idx, 0},
    {Disassembler::lds, Disassembler::idx, 0},
    {Disassembler::sts, Disassembler::idx, 0},
    {Disassembler::suba, Disassembler::ext, 0},
    {Disassembler::cmpa, Disassembler::ext, 0},
    {Disassembler::sbca, Disassembler::ext, 0},
    {Disassembler::subd, Disassembler::ext, 1},
    /* b0 */
    {Disassembler::anda, Disassembler::ext, 0},
    {Disassembler::bita, Disassembler::ext, 0},
    {Disassembler::lda, Disassembler::ext, 0},
    {Disassembler::sta, Disassembler::ext, 0},
    {Disassembler::eora, Disassembler::ext, 0},
    {Disassembler::adca, Disassembler::ext, 0},
    {Disassembler::ora, Disassembler::ext, 0},
    {Disassembler::adda, Disassembler::ext, 0},
    {Disassembler::cmpx, Disassembler::ext, 0},
    {Disassembler::jsr, Disassembler::ext, 0},
    {Disassembler::lds, Disassembler::ext, 0},
    {Disassembler::sts, Disassembler::ext, 0},
    {Disassembler::subb, Disassembler::imb, 0},
    {Disassembler::cmpb, Disassembler::imb, 0},
    {Disassembler::sbcb, Disassembler::imb, 0},
    {Disassembler::addd, Disassembler::imw, 1},
    /* c0 */
    {Disassembler::andb, Disassembler::imb, 0},
    {Disassembler::bitb, Disassembler::imb, 0},
    {Disassembler::ldb, Disassembler::imb, 0},
    {Disassembler::stb, Disassembler::imb, 7},
    {Disassembler::eorb, Disassembler::imb, 0},
    {Disassembler::adcb, Disassembler::imb, 0},
    {Disassembler::orb, Disassembler::imb, 0},
    {Disassembler::addb, Disassembler::imb, 0},
    {Disassembler::ldd, Disassembler::imw, 1},
    {Disassembler::_std, Disassembler::imw, 1},
    {Disassembler::ldx, Disassembler::imw, 0},
    {Disassembler::stx, Disassembler::imw, 7},
    {Disassembler::subb, Disassembler::dir, 0},
    {Disassembler::cmpb, Disassembler::dir, 0},
    {Disassembler::sbcb, Disassembler::dir, 0},
    {Disassembler::addd, Disassembler::dir, 1},
    /* d0 */
    {Disassembler::andb, Disassembler::dir, 0},
    {Disassembler::bitb, Disassembler::dir, 0},
    {Disassembler::ldb, Disassembler::dir, 0},
    {Disassembler::stb, Disassembler::dir, 0},
    {Disassembler::eorb, Disassembler::dir, 0},
    {Disassembler::adcb, Disassembler::dir, 0},
    {Disassembler::orb, Disassembler::dir, 0},
    {Disassembler::addb, Disassembler::dir, 0},
    {Disassembler::ldd, Disassembler::dir, 1},
    {Disassembler::_std, Disassembler::dir, 1},
    {Disassembler::ldx, Disassembler::dir, 0},
    {Disassembler::stx, Disassembler::dir, 0},
    {Disassembler::subb, Disassembler::idx, 0},
    {Disassembler::cmpb, Disassembler::idx, 0},
    {Disassembler::sbcb, Disassembler::idx, 0},
    {Disassembler::addd, Disassembler::idx, 1},
    /* e0 */
    {Disassembler::andb, Disassembler::idx, 0},
    {Disassembler::bitb, Disassembler::idx, 0},
    {Disassembler::ldb, Disassembler::idx, 0},
    {Disassembler::stb, Disassembler::idx, 0},
    {Disassembler::eorb, Disassembler::idx, 0},
    {Disassembler::adcb, Disassembler::idx, 0},
    {Disassembler::orb, Disassembler::idx, 0},
    {Disassembler::addb, Disassembler::idx, 0},
    {Disassembler::ldd, Disassembler::idx, 1},
    {Disassembler::_std, Disassembler::idx, 1},
    {Disassembler::ldx, Disassembler::idx, 0},
    {Disassembler::stx, Disassembler::idx, 0},
    {Disassembler::subb, Disassembler::ext, 0},
    {Disassembler::cmpb, Disassembler::ext, 0},
    {Disassembler::sbcb, Disassembler::ext, 0},
    {Disassembler::addd, Disassembler::ext, 1},
    /* f0 */
    {Disassembler::andb, Disassembler::ext, 0},
    {Disassembler::bitb, Disassembler::ext, 0},
    {Disassembler::ldb, Disassembler::ext, 0},
    {Disassembler::stb, Disassembler::ext, 0},
    {Disassembler::eorb, Disassembler::ext, 0},
    {Disassembler::adcb, Disassembler::ext, 0},
    {Disassembler::orb, Disassembler::ext, 0},
    {Disassembler::addb, Disassembler::ext, 0},
    {Disassembler::ldd, Disassembler::ext, 1},
    {Disassembler::_std, Disassembler::ext, 1},
    {Disassembler::ldx, Disassembler::ext, 0},
    {Disassembler::stx, Disassembler::ext, 0},

    /* extra instruction $fc for NSC-8105 */
    {Disassembler::addx, Disassembler::ext, 0},
    /* extra instruction $ec for NSC-8105 */
    {Disassembler::adcx, Disassembler::imb, 0}};



    int Disassembler::SIGNED(int b)
    {
        return ((int)((b & 0x80) == 0x80 ? b | 0xffffff00 : b));
    }

     bool Disassembler::IsSubroutine(int opcode)
    {
        return opcode == bsr || opcode == jsr;
    }

     bool Disassembler::IsReturn(int opcode)
    {
        return opcode == rti || opcode == rts;
    }

 DasmResult Disassembler::disassemble(uint8_t *memory, int address)
    {
        static char *instruction = (char *)malloc(8);
        static char *operand = (char *)malloc(8);

        int flags = 0;
        int invalid_mask;
        int code = memory[0] & 0xff;
        int opcode, args, invalid;

        invalid_mask = 1;

        opcode = table[code][0];
        args = table[code][1];
        invalid = table[code][2];

        if (IsSubroutine(opcode))
            flags = DASMFLAG_STEP_OVER;
        else if (IsReturn(opcode))
            flags = DASMFLAG_STEP_OUT;

        bool is_illegal = false;
        // char *operand = nullptr;
        // char *instruction = nullptr;
        int byteLength = 0;

        if ((invalid & invalid_mask) == invalid_mask) /* invalid for this cpu type ? */
        {
            return DasmResult{true, nullptr, nullptr, flags | DASMFLAG_SUPPORTED, 1};
        }

        sprintf(instruction, "%-4s", op_name_str[opcode]);

        // int byteLength = 0;

        // string operand = "";

        switch (args)
        {
        case rel: /* relative */
            sprintf(operand, "$%04x", address + SIGNED(memory[1]) + 2);
            byteLength = 2;
            break;
        case imb: /* immediate (byte) */
            sprintf(operand, "#$%02x", memory[1]);
            byteLength = 2;
            break;
        case imw: /* immediate (word) */
            sprintf(operand, "#$%04x", (memory[1] << 8) + memory[2]);
            byteLength = 3;
            break;
        case idx: /* indexed + byte offset */
            sprintf(operand, "$%02x,x", memory[1]);
            byteLength = 2;
            break;
        case imx: /* immediate, indexed + byte offset */
            sprintf(operand, "#$%02x,(x+$%02x)", memory[1], memory[2]);
            byteLength = 3;
            break;
        case dir: /* direct address */
            sprintf(operand, "$%02x", memory[1]);
            byteLength = 2;
            break;
        case imd: /* immediate, direct address */
            sprintf(operand, "#$%02x,$%02x", memory[1], memory[2]);
            byteLength = 3;
            break;
        case ext: /* extended address */
            sprintf(operand, "$%04x", (memory[1] << 8) + memory[2]);
            byteLength = 3;
            break;
        case sx1: /* byte from address (s + 1) */
            sprintf(operand, "(s+1)");
            byteLength = 1;
            break;
        default:
            byteLength = 1;
            break;
        }

        return DasmResult{
            is_illegal,
            operand,
            instruction,
            byteLength,
            flags | DASMFLAG_SUPPORTED};
        // return new DasmResult(){
        //     Instruction = instruction,
        //     Operand = operand,
        //     ByteLength = byteLength,
        //     Flags = flags | DASMFLAG_SUPPORTED};
    }

    //     static int Disassemble(int[] memory, int pc, ref string buf)
    //     {
    //         int flags = 0;
    //         int invalid_mask;
    //         int code = memory[pc] & 0xff;
    //         int opcode, args, invalid;

    //         invalid_mask = 1;

    //         opcode = table[code][0];
    //         args = table[code][1];
    //         invalid = table[code][2];

    //         if (IsSubroutine(opcode))
    //             flags = DASMFLAG_STEP_OVER;
    //         else if (IsReturn(opcode))
    //             flags = DASMFLAG_STEP_OUT;

    //         if ((invalid & invalid_mask) == invalid_mask) /* invalid for this cpu type ? */
    //         {
    //             buf += "illegal";
    //             return 1 | flags | DASMFLAG_SUPPORTED;
    //         }

    //         buf += string.Format("{0,-4} ", op_name_str[opcode]);

    //         switch (args)
    //         {
    //         case rel: /* relative */
    //             buf += string.Format("${0:X4}", pc + SIGNED(memory[pc + 1]) + 2);
    //             return 2 | flags | DASMFLAG_SUPPORTED;
    //         case imb: /* immediate (byte) */
    //             buf += string.Format("#${0:X2}", memory[pc + 1]);
    //             return 2 | flags | DASMFLAG_SUPPORTED;
    //         case imw: /* immediate (word) */
    //             buf += string.Format("#${0:X4}", (memory[pc + 1] << 8) + memory[pc + 2]);
    //             return 3 | flags | DASMFLAG_SUPPORTED;
    //         case idx: /* indexed + byte offset */
    //             buf += string.Format("${0:X2},x", memory[pc + 1]);
    //             return 2 | flags | DASMFLAG_SUPPORTED;
    //         case imx: /* immediate, indexed + byte offset */
    //             buf += string.Format("#${0:X2},(x+${1:X2})", memory[pc + 1], memory[pc + 2]);
    //             return 3 | flags | DASMFLAG_SUPPORTED;
    //         case dir: /* direct address */
    //             buf += string.Format("${0:X2}", memory[pc + 1]);
    //             return 2 | flags | DASMFLAG_SUPPORTED;
    //         case imd: /* immediate, direct address */
    //             buf += string.Format("#${0:X2},${1:X2}", memory[pc + 1], memory[pc + 2]);
    //             return 3 | flags | DASMFLAG_SUPPORTED;
    //         case ext: /* extended address */
    //             buf += string.Format("${0:X4}", (memory[pc + 1] << 8) + memory[pc + 2]);
    //             return 3 | flags | DASMFLAG_SUPPORTED;
    //         case sx1: /* byte from address (s + 1) */
    //             buf += string.Format("(s+1)");
    //             return 1 | flags | DASMFLAG_SUPPORTED;
    //         default:
    //             return 1 | flags | DASMFLAG_SUPPORTED;
    //         }
    //     }

    // public
    //     static void SelfTest()
    //     {
    //         for (int i = 0; i < table.Length; i++)
    //         {
    //             var entry = table[i];
    //             //if ((entry[2] & 1) == entry[2])
    //             //{
    //             if (!((IList)valid6800opcodes).Contains(i))
    //             {
    //                 // this is an invalid opcode
    //                 Debug.WriteLine("{0:X2}: {1}", i, entry[2]);
    //             }
    //             //}
    //         }
    //     }
#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H
#include <stdint.h>

using offs_t = uint32_t;

struct CpuStatus {
	uint32_t pc;
	uint32_t sp;
	uint32_t ix;
	uint8_t acca;
	uint8_t accb;
	uint8_t cc;
};

class et3400emu;

#endif // COMMON_DEFS_H
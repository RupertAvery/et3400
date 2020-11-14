#ifndef CPU_DEFS_H
#define CPU_DEFS_H
#include <stdint.h>

#define logerror(x, y, z) printf(x, y, z);

#define LSB_FIRST

// I/O line states
enum line_state
{
	CLEAR_LINE = 0, // clear (a fired or held) line
	ASSERT_LINE,	// assert an interrupt immediately
	HOLD_LINE		// hold interrupt line until acknowledged
};

// I/O line definitions
enum
{
	// input lines
	MAX_INPUT_LINES = 64 + 3,
	INPUT_LINE_IRQ0 = 0,
	INPUT_LINE_IRQ1 = 1,
	INPUT_LINE_IRQ2 = 2,
	INPUT_LINE_IRQ3 = 3,
	INPUT_LINE_IRQ4 = 4,
	INPUT_LINE_IRQ5 = 5,
	INPUT_LINE_IRQ6 = 6,
	INPUT_LINE_IRQ7 = 7,
	INPUT_LINE_IRQ8 = 8,
	INPUT_LINE_IRQ9 = 9,
	INPUT_LINE_NMI = MAX_INPUT_LINES - 3,

	// special input lines that are implemented in the core
	INPUT_LINE_RESET = MAX_INPUT_LINES - 2,
	INPUT_LINE_HALT = MAX_INPUT_LINES - 1
};

union PAIR
{
#ifdef LSB_FIRST
	struct
	{
		uint8_t l, h, h2, h3;
	} b;
	struct
	{
		uint16_t l, h;
	} w;
	struct
	{
		int8_t l, h, h2, h3;
	} sb;
	struct
	{
		int16_t l, h;
	} sw;
#else
	struct
	{
		uint8_t h3, h2, h, l;
	} b;
	struct
	{
		int8_t h3, h2, h, l;
	} sb;
	struct
	{
		uint16_t h, l;
	} w;
	struct
	{
		int16_t h, l;
	} sw;
#endif
	uint32_t d;
	int32_t sd;
};


enum
{
	M6800_PC = 1,
	M6800_S,
	M6800_A,
	M6800_B,
	M6800_X,
	M6800_CC,
	M6800_WAI_STATE
};

enum
{
	M6800_IRQ_LINE = 0 /* IRQ line number */
};

enum
{
	M6802_IRQ_LINE = M6800_IRQ_LINE
};

enum
{
	M6808_IRQ_LINE = M6800_IRQ_LINE
};

#endif // CPU_DEFS_H
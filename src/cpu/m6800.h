// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*** m6800: Portable 6800 class emulator *************************************/

#ifndef MAME_CPU_M6800_M6800_H
#define MAME_CPU_M6800_M6800_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "defs.h"
#include "../dev/memory_map.h"
#include <functional>
#include <vector>
#include <mutex>

extern std::mutex bplocks;
// class cpu_device
// {
// 	virtual void device_start();
// 	virtual void device_reset();

// 	// device_execute_interface overrides
// 	virtual uint32_t execute_min_cycles() const noexcept { return 1; }
// 	virtual uint32_t execute_max_cycles() const noexcept { return 12; }
// 	virtual uint32_t execute_input_lines() const noexcept { return 2; }
// 	virtual bool execute_input_edge_triggered(int inputnum) const noexcept { return inputnum == INPUT_LINE_NMI; }
// 	virtual void execute_run();
// 	virtual void execute_set_input(int inputnum, int state);
// };

class m6800_cpu_device
{
public:
	typedef void (m6800_cpu_device::*op_func)();

	// construction/destruction
	// m6800_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	m6800_cpu_device(memory_map *memory_map, std::vector<BreakPoint> *breakpoints);

	enum
	{
		M6800_WAI = 8,	 /* set when WAI is waiting for an interrupt */
		M6800_SLP = 0x10 /* HD63701 only */
	};

	// m6800_cpu_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, const m6800_cpu_device::op_func *insn, const uint8_t *cycles, address_map_constructor internal);

	// device-level overrides
	void device_start();
	void device_reset();

	// device_execute_interface overrides
	uint32_t execute_min_cycles() const noexcept { return 1; }
	uint32_t execute_max_cycles() const noexcept { return 12; }
	uint32_t execute_input_lines() const noexcept { return 2; }
	bool execute_input_edge_triggered(int inputnum) const noexcept { return inputnum == INPUT_LINE_NMI; }
	void execute_run(bool resume = false);
	void execute_step();
	void execute_set_input(int inputnum, int state);
	void pre_execute_run();
	std::vector<BreakPoint> *breakpoints;
	bool has_breakpoint(offs_t);

	CpuStatus get_status();
	// device_memory_interface overrides
	//virtual space_config_vector memory_space_config() const override;

	// device_state_interface overrides
	//virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	// device_disasm_interface overrides
	// virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

	// address_space_config m_program_config;
	// address_space_config m_decrypted_opcodes_config;
	// address_space_config m_io_config;

	PAIR m_ppc;				/* Previous program counter */
	PAIR m_pc;				/* Program counter */
	PAIR m_s;				/* Stack pointer */
	PAIR m_x;				/* Index register */
	PAIR m_d;				/* Accumulators */
	uint8_t m_cc;			/* Condition codes */
	uint8_t m_wai_state;	/* WAI opcode state ,(or sleep opcode state) */
	uint8_t m_nmi_state;	/* NMI line state */
	uint8_t m_nmi_pending;	/* NMI pending */
	uint8_t m_irq_state[3]; /* IRQ line state [IRQ1,TIN,SC1] */

	/* Memory spaces */
	// memory_access<16, 0, 0, ENDIANNESS_BIG>::cache m_cprogram, m_copcodes;
	// memory_access<16, 0, 0, ENDIANNESS_BIG>::specific m_program;

	memory_map *memory_map;
	uint8_t read_byte(offs_t address);
	uint8_t *read_bytes(offs_t address, size_t size);
	void write_byte(offs_t address, uint8_t data);
	void write_bytes(offs_t address, uint8_t *data, size_t size);

	const op_func *m_insn;
	const uint8_t *m_cycles; /* clock cycle of instruction table */
	int m_icount;
	int reset_line;
	bool verbose;
	bool is_break;

protected:
	PAIR m_ea; /* effective address */

	static const uint8_t flags8i[256];
	static const uint8_t flags8d[256];
	static const uint8_t cycles_6800[256];
	static const uint8_t cycles_nsc8105[256];
	static const op_func m6800_insn[256];
	static const op_func nsc8105_insn[256];

	uint32_t RM16(uint32_t Addr);
	void WM16(uint32_t Addr, PAIR *p);
	void enter_interrupt(const char *message, uint16_t irq_vector);
	virtual void m6800_check_irq2() {}
	void CHECK_IRQ_LINES();
	virtual void increment_counter(int amount);
	virtual void EAT_CYCLES();
	virtual void CLEANUP_COUNTERS() {}
	virtual void TAKE_TRAP() {}

	void aba();
	void abx();
	void adca_di();
	void adca_ex();
	void adca_im();
	void adca_ix();
	void adcb_di();
	void adcb_ex();
	void adcb_im();
	void adcb_ix();
	void adcx_im();
	void adda_di();
	void adda_ex();
	void adda_im();
	void adda_ix();
	void addb_di();
	void addb_ex();
	void addb_im();
	void addb_ix();
	void addd_di();
	void addd_ex();
	void addx_ex();
	void addd_im();
	void addd_ix();
	void aim_di();
	void aim_ix();
	void anda_di();
	void anda_ex();
	void anda_im();
	void anda_ix();
	void andb_di();
	void andb_ex();
	void andb_im();
	void andb_ix();
	void asl_ex();
	void asl_ix();
	void asla();
	void aslb();
	void asld();
	void asr_ex();
	void asr_ix();
	void asra();
	void asrb();
	void bcc();
	void bcs();
	void beq();
	void bge();
	void bgt();
	void bhi();
	void bita_di();
	void bita_ex();
	void bita_im();
	void bita_ix();
	void bitb_di();
	void bitb_ex();
	void bitb_im();
	void bitb_ix();
	void ble();
	void bls();
	void blt();
	void bmi();
	void bne();
	void bpl();
	void bra();
	void brn();
	void bsr();
	void bvc();
	void bvs();
	void cba();
	void clc();
	void cli();
	void clr_ex();
	void clr_ix();
	void clra();
	void clrb();
	void clv();
	void cmpa_di();
	void cmpa_ex();
	void cmpa_im();
	void cmpa_ix();
	void cmpb_di();
	void cmpb_ex();
	void cmpb_im();
	void cmpb_ix();
	void cmpx_di();
	void cmpx_ex();
	void cmpx_im();
	void cmpx_ix();
	void com_ex();
	void com_ix();
	void coma();
	void comb();
	void daa();
	void dec_ex();
	void dec_ix();
	void deca();
	void decb();
	void des();
	void dex();
	void eim_di();
	void eim_ix();
	void eora_di();
	void eora_ex();
	void eora_im();
	void eora_ix();
	void eorb_di();
	void eorb_ex();
	void eorb_im();
	void eorb_ix();
	void illegl1();
	void illegl2();
	void illegl3();
	void inc_ex();
	void inc_ix();
	void inca();
	void incb();
	void ins();
	void inx();
	void jmp_ex();
	void jmp_ix();
	void jsr_di();
	void jsr_ex();
	void jsr_ix();
	void lda_di();
	void lda_ex();
	void lda_im();
	void lda_ix();
	void ldb_di();
	void ldb_ex();
	void ldb_im();
	void ldb_ix();
	void ldd_di();
	void ldd_ex();
	void ldd_im();
	void ldd_ix();
	void lds_di();
	void lds_ex();
	void lds_im();
	void lds_ix();
	void ldx_di();
	void ldx_ex();
	void ldx_im();
	void ldx_ix();
	void lsr_ex();
	void lsr_ix();
	void lsra();
	void lsrb();
	void lsrd();
	void mul();
	void neg_ex();
	void neg_ix();
	void nega();
	void negb();
	void nop();
	void oim_di();
	void oim_ix();
	void ora_di();
	void ora_ex();
	void ora_im();
	void ora_ix();
	void orb_di();
	void orb_ex();
	void orb_im();
	void orb_ix();
	void psha();
	void pshb();
	void pshx();
	void pula();
	void pulb();
	void pulx();
	void rol_ex();
	void rol_ix();
	void rola();
	void rolb();
	void ror_ex();
	void ror_ix();
	void rora();
	void rorb();
	void rti();
	void rts();
	void sba();
	void sbca_di();
	void sbca_ex();
	void sbca_im();
	void sbca_ix();
	void sbcb_di();
	void sbcb_ex();
	void sbcb_im();
	void sbcb_ix();
	void sec();
	void sei();
	void sev();
	void slp();
	void sta_di();
	void sta_ex();
	void sta_im();
	void sta_ix();
	void stb_di();
	void stb_ex();
	void stb_im();
	void stb_ix();
	void std_di();
	void std_ex();
	void std_im();
	void std_ix();
	void sts_di();
	void sts_ex();
	void sts_im();
	void sts_ix();
	void stx_di();
	void stx_ex();
	void stx_im();
	void stx_ix();
	void suba_di();
	void suba_ex();
	void suba_im();
	void suba_ix();
	void subb_di();
	void subb_ex();
	void subb_im();
	void subb_ix();
	void subd_di();
	void subd_ex();
	void subd_im();
	void subd_ix();
	void swi();
	void tab();
	void tap();
	void tba();
	void tim_di();
	void tim_ix();
	void tpa();
	void tst_ex();
	void tst_ix();
	void tsta();
	void tstb();
	void tsx();
	void txs();
	void undoc1();
	void undoc2();
	void wai();
	void xgdx();
	void cpx_di();
	void cpx_ex();
	void cpx_im();
	void cpx_ix();
	void trap();
	void btst_ix();
	void stx_nsc();
};

#endif // MAME_CPU_M6800_M6800_H

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Feedback Shift Register (FSR) Display Demo for the Heathkit ET-3400
; Microprocessor Trainer, by Rick Nungester 6/22/2010.
;
; This program provides a display of a linear feedback shift register.
; (See http://en.wikipedia.org/wiki/Linear_feedback_shift_register.)
; For example, a 16-bit FSR with feedback taps at bits 14 and 15, and
; seed value 1, behaves like this:
;                    _____
;     +------------>|     |
;     |             | XOR |---------------+
;     | +---------->|_____|               |
;     | |                                 |
;     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 <---+
;     0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0
;     0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0
;     ...
;     0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0
;     0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0
;     0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0
;     1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 <<< key transition
;     0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1
;     0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0
;     0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0
;     ...
;      14  12  10 9 8 7 6 5 4 3 2 1 0 <<< bit #
;    15  13  11
;
; The initial value of the register is $0001.  Each left-shift, the
; new bit 0 value is bits 14 and 15 exclusive or'd together.  This
; particular FSR, including its seed of 1, has a repeating cycle of
; 255 states.
;
; The FSR bits are mapped to Heathkit ET-3400 LED segments so the
; operation of the FSR can be seen.  All the following can be
; manually changed after the program is loaded into the ET-3400:
; FSR feedback taps (FSRTAPS), FSR seed value (FSRSEED), mapping
; of FSR bits to LED segments (FSRTBL), and output delay for each
; LED display update (FSRWAIT).
;
; The FSR is a fixed 32 bits in length, but not all bits need to
; be mapped to LED segments.  For example, if taps are chosen at
; bits 0 and 23, an attractive pattern is produced if only bits 0
; through 23 are mapped to LED segments, and FSR bits 24 through
; 31 are ignored.
;
; To load and execute the program, your ET-3400 must include the
; optional 256 bytes of RAM in the sockets labeled "RAM (0100)"
; at the upper-left of the board.
;
; This program was inspired by a similar demo program in the Hewlett-
; Packard 5036A Microprocessor Lab, a one-board trainer based on the
; Intel 8085 microprocessor.  (That trainer helped trigger my
; career switch from electrical engineering to software engineering
; in 1979.)
;
; Nice patterns on the ET-3400 (n bits long), tend to have 2 taps, at
; bits n-1 and n-2, or at n-1 and 0.  This leaves most of the FSR bits
; (LED segments) untapped, so the bits can be seen "flowing nicely"
; through those bit postions.  Also, 1 makes a good seed because it
; starts the LED pattern trace with either a single segment (n-1, n-2
; feedback) or a "line being drawn" (n-1, 0 feedback).  Cycles that
; are fairly short (under 1000) are best because they repeat in
; reasonable time.
;
; Good patterns: Feedback at bits 15 and 14, or 15 and 0, seed = 1,
; makes a cycle length of 255 states.  (This is the image seen on
; the HP 5036A Microprocessro Lab demo, inverted because there a
; bit value of 1 means LED segment off instead of on.)  Feedback 
; at bits 31 and 30, or 31 and 0, seed = 1, makes a cycle length of
; 1023 states.  15,11,8,6 feedback makes the FSR used in HP Signature
; Analyzers, with a cycle length of 2^16-1 = 65535.  31,21,1,0 makes
; a cycle length of 2^32-1 = 4,294,967,295, which would take about
; 14 years to repeat with the default display timing of 100 ms per
; state.
;
; Development used the TASM cross-assembler(home.comcast.net/~tasm/)
; and the ET-3400 simulator ("Trainer Simulator" version 1.1 from
; Pilgrim Works, Bill Lovegrove, www.pilgrimworks.com).  The simulator
; is no longer supported or available from Pilgrim Works, but is
; available from tech.groups.yahoo.com/group/ET-3400/.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ET-3400 constants
;
USRRAM0 .equ    $0000 ; 197 bytes user RAM ($0000-$00C4)
USRRAM1 .equ    $0100 ; 256 bytes user RAM ($0100-$01FF)
REDIS   .equ    $FCBC ; REset DISplays for successive OUTCH calls
OUTCH   .equ    $FE3A ; OUTput CHaracter to a 7-seg display

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FSRDEMO is the main program, here at address $0000 for ease of
; remembering and entering on the ET-3400 keypad.  This is the
; only code I put in USRRAM0, and all the rest is in USRRAM1
; (taking up almost all its 256 bytes).
;
        .org    USRRAM0 ; All program code is too big for this RAM block
        .msfirst        ; make TASM assembler ".word" do MC6800 byte ordering
;
FSRDEMO JSR     FSRINIT ; Do 1-time initialization
FSRLOOP JSR     FSRMAP  ; Map FSR bits to LED display segments
        JSR     FSROUT  ; Output to LED displays
        JSR     FSRDLY  ; Pause to see one output pattern
        JSR     FSRSHFT ; Shift the FSR to make a new FSR value
        BRA     FSRLOOP ; Repeat

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Constants, Lookup Tables, and Variables.  (The TASM assembler uses
; .byte and .word to define values in memory, and .block to set aside
; uninitialized space.)
;
; global vars
FSRVAL  .block  4       ; FSR value, 32-bits
LEDS    .block  6       ; 6 LED displays, OUTCH data, right-to-left

; global constants
FSRP2   .byte   $01,$02,$04,$08
        .byte   $10,$20,$40,$80 ; 2^0..2^7 bitmasks

; FRSINIT constant
FSRSEED .word   $0000,$0001

; FSRGET vars
FSRBYTE .block  1       ; one byte of the FSR

; FSRSHFT vars, constants
FSRBIT0 .block  1       ; 0 or 1, next FSR bit 0 value after shift
FSRTAPC .block  1       ; FSR feedback tap counter
FSRTAPS .byte   0       ; FSR feedback taps (6 max)
        .byte   15
FSRTAPE .equ    $       ; table end address, for finding table length
        .block  6-(FSRTAPE-FSRTAPS) ; extra for experiments, 6 max
FSRTAPN .byte   FSRTAPE-FSRTAPS ; number of taps for program to use

; FSRDEL constant
FSRWAIT .word   $0001   ; display delay (ET-3400 $1008, simulator $0001)

; FSRMAP vars
BITCNT  .block  1       ; FSRVAL bit index counter
FSRBIT  .block  1       ; one bit of the FSR ($00 or $01)
FSRDEST .block  1       ; destination of FSRBIT in LEDS
FSRMAPX .block  2       ; saved index register
;
; FSRTBL defines the mapping of FSR bits to ET-3400 display segments.
; Alternate patterns are given in a separate file.  Given the
; required ET-3400 OUTCH (OUTput CHaracter) bit mapping of
;      6
;    +---+
;   1|   |5
;    +-0-+
;   2|   |4
;    +---+ .7
;      3
; and the desired mapping of FSR bits to display segments in this
; 16-bit pattern as used on HP 5036A Microprocessor Lab (except that
; its defect of having bits 8..15 in positions 9..15,8 below, is fixed):
;     10        9         1         2
;   +---+     +---+     +---+     +---+     +---+     +---+
; 11|   |     |   |8   0|   |     |   |3    |   |     |   |
;   +---+     +---+     +---+     +---+     +---+     +---+
; 12|   |     |   |15  7|   |     |   |4    |   |     |   |
;   +---+     +---+     +---+     +---+     +---+     +---+
;     13        14        6         5
; the resulting mapping of FSR bits to digit (d) and bit (b) is
;                  dddbbb ; digit (5 left, 0 right) and bit (7 MS, 0 LS)
FSRTBL  .byte   %00011001 ; FSR bit  0 maps to digit 3, segment 1
        .byte   %00011110 ;          1
        .byte   %00010110 ;          2
        .byte   %00010101 ;          3
        .byte   %00010100 ;          4
        .byte   %00010011 ;          5
        .byte   %00011011 ;          6
        .byte   %00011010 ;          7
        .byte   %00100101 ;          8
        .byte   %00100110 ;          9
        .byte   %00101110 ;         10
        .byte   %00101001 ;         11
        .byte   %00101010 ;         12
        .byte   %00101011 ;         13
        .byte   %00100011 ;         14
        .byte   %00100100 ; FSR bit 15 maps to digit 4, segment 4
FSRTBL1 .equ    $         ; table end address, for finding table length
;
; Pad FSRTBL to 32 bytes total, for manual experimentation after the
; program is loaded into the ET-3400.  FSRTBL2 gives the actual number
; of table bytes for the program to use.  If the table is manually
; extended, the value of FSRTBL2 must be increased accordingly.
;
        .block  32-(FSRTBL1-FSRTBL)
FSRTBL2 .byte   FSRTBL1-FSRTBL  ; table size used by the program

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; All subroutines follow, taking up almost all of the optional ET-3400
; RAM space (256 bytes).
;
        .org    USRRAM1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FSRINIT does one-time setup before the program's infinite loop.
;
FSRINIT CLR     LEDS+0  ; Init all LEDS values, all segments off
        CLR     LEDS+1
        CLR     LEDS+2
        CLR     LEDS+3
        CLR     LEDS+4
        CLR     LEDS+5
        JSR     FSROUT

        LDAA    FSRSEED+3 ; Initialize FSR seed
        STAA    FSRVAL+0  ; LS byte
        LDAA    FSRSEED+2
        STAA    FSRVAL+1
        LDAA    FSRSEED+1
        STAA    FSRVAL+2
        LDAA    FSRSEED+0
        STAA    FSRVAL+3  ; MS byte
        RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FSRMAP maps the FSR bits (FSRVAL, 32 bits max) to their designated LED
; display segments (LEDS, 6 bytes, 48 possible LED segments), based on
; the assignments in table FSRTBL.
;
FSRMAP  CLRA
        STAA    BITCNT    ; A = BITCNT = 0

FSRMAP1 JSR     FSRGET    ; A = BITCNT, fetch that FSR bit
        STAA    FSRBIT    ; FSRBIT = 0 or 1

        LDAA    BITCNT    ; Find FSRBIT's destination in LEDS
        LDX     #FSRTBL-1
FSRMAP2 INX
        DECA
        BPL     FSRMAP2
        LDAA    0,X
        STAA    FSRDEST   ; FSRDEST = 00dddbbb LEDS destination
        LSRA
        LSRA
        LSRA              ; A = byte offset into LEDS (0..5)
        LDX     #LEDS-1
FSRMAP3 INX
        DECA
        BPL     FSRMAP3
        STX     FSRMAPX   ; FSRMAPX = address of destination LEDS byte

        LDAA    FSRDEST
        ANDA    #$07      ; A = bit # of destination bit
        LDX     #FSRP2-1
FSRMAP4 INX
        DECA
        BPL     FSRMAP4
        LDAA    0,X       ; A = bit mask for LEDS (1 1-bit, 7 0-bits)

        LDX     FSRMAPX   ; X = addr of LEDS byte to merge back into
        TST     FSRBIT    ; Clearing (off) or setting (on) an LED segment?
        BEQ     FSRMAP5   ; If clearing an LEDS bit, branch

        ORAA    0,X       ; Set the desired LEDS bit/segment
        BRA     FSRMAP6

FSRMAP5 COMA              ; Clear the desired LEDS bit/segment
        ANDA    0,X

FSRMAP6 STAA    0,X       ; Store the merged-in segment into LEDS
        INC     BITCNT    ; Done with all desired FSR bit mappings?
        LDAA    BITCNT
        CMPA    FSRTBL2
        BLT     FSRMAP1   ; If not, branch up (with A = BITCNT)
        RTS               ; If so, return

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FSROUT outputs all 6 7-segment LED displays from the contents of the
; LEDS bytes.
;
FSROUT  JSR     REDIS   ; prepare for output to leftmost 7-seg display
        LDAA    LEDS+5
        JSR     OUTCH   ; leftmost display of 6
        LDAA    LEDS+4
        JSR     OUTCH
        LDAA    LEDS+3
        JSR     OUTCH
        LDAA    LEDS+2
        JSR     OUTCH
        LDAA    LEDS+1
        JSR     OUTCH
        LDAA    LEDS+0
        JSR     OUTCH   ; rightmost display of 6
        RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FRSDLY delays to see one output pattern.
;
FSRDLY  LDX     FSRWAIT
FSRDLY1 DEX
        BNE     FSRDLY1
        RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FSRSHFT does 1 left-shift of the 32-bit FSR, shifting into bit #0 the
; XOR of the feedback taps defined in table FSRTAPS.
;
FSRSHFT CLR     FSRBIT0   ; Define bit #0 of next FSR state.
        CLR     FSRTAPC
        CLRA              ; A = FSRTAPC = 0 = feedback tap counter

FSRSHF1 LDX     #FSRTAPS-1 ; Grab FSRTAPS[FSRTAPC] table entry
FSRSHF2 INX
        DECA
        BPL     FSRSHF2

        LDAA    0,X       ; A = feedback tap bit #
        JSR     FSRGET    ; A = feedback tap bit (0 or 1)
        ADDA    FSRBIT0
        STAA    FSRBIT0   ; FSRBIT0 = sum of feedback bits so far
        INC     FSRTAPC   ; FSRTAPC = # feedback bits summed
        LDAA    FSRTAPC
        CMPA    FSRTAPN   ; Done summing all feedback bits?
        BNE     FSRSHF1   ; If not, branch up (A = FSRTAPC)

        LDAA    FSRBIT0   ; A = sum of all feedback tap bits
        ANDA    #1
        STAA    FSRBIT0   ; FSRBIT0 = XOR of all feedback taps (done)

        ASL    FSRVAL+0   ; shift 32-bit FSR left one bit

; There is a bug in the ET-3400 simulator that I am using to develop this
; program.  The trainer is no longer supported, and the bug is being
; avoided.  The simulator bug is that "ROL <address>" does not rotate
; through the Carry bit as it should.  To demonstrate, do the following
; on the simulator and ET-3400 and see different results.  RESET, AUTO,
; 0000, 00, 0d, 79, 00, 00, RESET, PC, CHAN, 0001, SS, SS, EXAM, 0000.
; This clears address $0000 to value $00, enters program "SEC, ROL $0000"
; at address $0001, then executes the two program steps. SEC sets the
; Carry Bit. The ROL should rotate this bit into the value at address
; $0000. The ET-3400 correctly shows "01" at address $0000, but the
; simulator incorrectly shows "00".
;
;       ROL    FSRVAL+1     ; THIS WON'T WORK ON THE SIMULATOR,
;       ROL    FSRVAL+2
;       ROL    FSRVAL+3     ; but what follows will.

        LDAA   FSRVAL+1
        ROLA
        STAA   FSRVAL+1

        LDAA   FSRVAL+2
        ROLA
        STAA   FSRVAL+2

        LDAA   FSRVAL+3
        ROLA
        STAA   FSRVAL+3

        LDAA   FSRBIT0      ; merge in the new bit 0
        ADDA   FSRVAL
        STAA   FSRVAL
        RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; FSRGET accepts an FSR bit # in ACC A, and returns that bit in ACC A
; right-justified (value $00 or $01).
;
FSRGET  TAB               ; B = desired FSR bit number, for later use
        LSRA
        LSRA
        LSRA              ; A = FSR byte # (0..3)
        LDX     #FSRVAL-1 ; Start one address back, for initial INX
FSRGET1 INX               ; first-pass, X points at FSRVAL (LS byte)
        DECA              ; decrement A, increment X, until right byte
        BPL     FSRGET1

        LDAA    0,X
        STAA    FSRBYTE   ; FSRBYTE contains the bit we want
        TBA               ; A = desired FSR bit # (0..31)
        ANDA    #$07      ; A = desired FSR bit # in FSRBYTE (0..7)
        LDX     #FSRP2-1
FSRGET2 INX
        DECA
        BPL     FSRGET2
        LDAB    0,X       ; B = bit mask to test desired FSR bit
        CLRA              ; assume return value = 0 until find otherwise
        BITB    FSRBYTE   ; desired FSR bit = 0?
        BEQ     FSRGET3   ; if so, branch (return 0)
        LDAA    #1        ; if not, return 1
FSRGET3 RTS

        .END

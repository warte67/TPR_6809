;  **********************************************
;  * Allocated 64k Memory Mapped System Symbols *
;  **********************************************


;  Zero-Page Kernal Variables:
SYSTEM_VARS     equ     $0010   ; start kernal vectors and variables

;  Stack Frames:
U_STK_BTM       equ     $0100   ; 256 bytes default user stack space
U_STK_TOP       equ     $0200   ; User Stack initial address
S_STK_BTM       equ     $0200   ; 512 bytes default system stack space
S_STK_TOP       equ     $0400   ; System Stack initial address

;  Video Buffer Memory (target = $0400):
VIDEO_START     equ     $0400   ; Start of Video Buffer Memory
VIDEO_END       equ     $17ff   ; Last Byte of Video Buffer Memory

;  Graphics Hardware Registers:
GFX_FLAGS       equ     $1800   ; (Byte) gfx system flags:
                                ;      bit 7: VSYNC
                                ;      bit 6: backbuffer enable
                                ;      bit 5: swap backbuffers (on write)
                                ;      bit 4: debug enable
                                ;      bits 2-3 = 'Background' graphics mode (40KB buffer)
                                ;          0) NONE (forced black background)
                                ;          1) Tiled 16x16 mode
                                ;          2) 256x160 x 64-Colors
                                ;          3) 512x320 x 4-Color
                                ;      bits 0-1 = 'Foreground' graphics mode (5KB buffer)
                                ;          0) 256x160 x 2-Color (with disable flag)
                                ;          1) Glyph Mode (32x20 text)
                                ;          2) Glyph Mode (64x40 text)
                                ;          3) 128x80 x 16-Color
GFX_AUX equ     $1801   ; (Byte) gfx auxillary/emulation flags:
                                ;      bit 7: 1:fullscreen / 0:windowed
                                ;      bit 6: reserved
                                ;      bit 5: reserved
                                ;      bit 4: reserved
                                ;      bit 3: reserved
                                ;      bit 0-2: monitor display index (0-7)
TIMING_WIDTH    equ     $1802   ; (Word) timing width
TIMING_HEIGHT   equ     $1804   ; (Word) timing height
GFX_PAL_INDX    equ     $1806   ; (Byte) gfx palette index (0-15)
GFX_PAL_DATA    equ     $1807   ; (Byte) gfx palette color bits r4g4b4a4

;  Paged Graphics Mode Hardware Registers:
GFX_PG_BEGIN    equ     $1808   ; start of paged gfxmode registers
GFX_EXT_ADDR    equ     $1808   ; (Word) 64K extended graphics addresses
GFX_EXT_DATA    equ     $180a   ; (Byte) 64K extended graphics RAM data
GFX_PG_END      equ     $180a   ; end of paged gfxmode registers

;  Mouse Cursor Hardware Registers:
CSR_XPOS        equ     $180b   ; (Word) horizontal mouse cursor coordinate
CSR_YPOS        equ     $180d   ; (Word) vertical mouse cursor coordinate
CSR_XOFS        equ     $180f   ; (Byte) horizontal mouse cursor offset
CSR_YOFS        equ     $1810   ; (Byte) vertical mouse cursor offset
CSR_SIZE        equ     $1811   ; (Byte) cursor size (0-15) 0:off
CSR_SCROLL      equ     $1812   ; (Signed) MouseWheel Scroll: -1, 0, 1
CSR_FLAGS       equ     $1813   ; (Byte) mouse button flags:
                                ;      bits 0-5: button states
                                ;      bits 6-7: number of clicks
CSR_PAL_INDX    equ     $1814   ; (Byte) mouse cursor color palette index (0-15)
CSR_PAL_DATA    equ     $1815   ; (Byte) mouse cursor color palette data RRGGBBAA
CSR_BMP_INDX    equ     $1816   ; (Byte) mouse cursor bitmap pixel offset
CSR_BMP_DATA    equ     $1817   ; (Byte) mouse cursor bitmap pixel color

GFX_END equ     $1817   ; end of the GFX Hardware Registers

RESERVED_HDW    equ     $1818   ; Reserved 2019 bytes ($1818 - $1FFB)

;  Memory Bank Selects (16MB):
RAMBANK_SEL_1   equ     $1ffc   ; (Word)Indexes 65536 x 8kb banks
RAMBANK_SEL_2   equ     $1ffe   ; (Word)Indexes 65536 x 8kb banks

;  Standard Usable (from FAST static 32KB) RAM:
RAM_START       equ     $2000   ; Begin System RAM (32k)
RAM_END equ     $9fff   ; End System RAM

;  Switchable RAM Banks (from SLOW external serial 16MB RAM chip):
RAM_BANK_1      equ     $a000   ; switched 8KB ram bank 1
RAM_BANK_2      equ     $c000   ; switched 8KB ram bank 2

;  Bios Kernal ROM:
BIOS_ROM        equ     $e000   ; Begin BIOS Kernal ROM (8KB)

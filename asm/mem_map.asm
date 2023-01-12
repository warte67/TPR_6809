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
                                ;      bit 7: vsync
                                ;      bit 6: backbuffer enable
                                ;      bit 5: debug enable
                                ;      bit 4: mouse cursor enable
                                ;      bit 3: swap backbuffers (on write)
                                ;      bit 0-2: graphics mode (0-7)
                                ;          0) NONE (just random background noise)
                                ;          1) Glyph Mode (512x320 or 64x40 text)
                                ;          2) Tile 16x16x16 mode
                                ;          3) 128x80 x 16-Color
                                ;          4) 128x160 x 4-Color
                                ;          5) 256x80 x 4-Color
                                ;          6) 256x160 x 2-Color
                                ;          7) 256x192 256-color  (EXTERNAL 64k BUFFER)
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
GFX_PAL_RED     equ     $1807   ; (Byte) red palette data (read/write)
GFX_PAL_GRN     equ     $1808   ; (Byte) grn palette data (read/write)
GFX_PAL_BLU     equ     $1809   ; (Byte) blu palette data (read/write)
GFX_PAL_ALF     equ     $180a   ; (Byte) alpha palette data (read/write)
                                ;    a special note
RESERVED_HDW    equ     $2000   ; Reserved ($2000-$1FFB)

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
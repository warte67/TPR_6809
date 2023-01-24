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
VIDEO_START     equ     $0400   ; Start of 5KB Video Buffer Memory
VIDEO_END       equ     $17ff   ; Last Byte of 5KB Video Buffer Memory

;  Graphics Hardware Registers:
GFX_BEGIN       equ     $1800   ; start of graphics hardware registers
GFX_CLK_DIV     equ     $1800   ; (Byte) 60 hz Clock Divider:
                                ;      bit 7: 0.46875 hz
                                ;      bit 6: 0.9375 hz
                                ;      bit 5: 1.875 hz
                                ;      bit 4: 3.75 hz
                                ;      bit 3: 7.5 hz
                                ;      bit 2: 15.0 hz
                                ;      bit 1: 30.0 hz
                                ;      bit 0: 60.0 hz
GFX_FLAGS       equ     $1801   ; (Byte) gfx system flags:
                                ;      bit 7: VSYNC
                                ;      bit 6: backbuffer enable
                                ;      bit 5: swap backbuffers (on write)
                                ;      bit 4: reserved
                                ;      bits 2-3 = 'Background' graphics modes (20KB buffer)
                                ;          0) NONE (forced black background)
                                ;          1) Tiled 16x16 mode
                                ;          2) Overscan Tile 16x16 mode
                                ;          3) 128x80 x 256-Colors
                                ;      bits 0-1 = 'Foreground' graphics modes (5KB buffer)
                                ;          0) 256x160 x 2-Color (with disable flag)
                                ;          1) Glyph Mode (32x20 text)
                                ;          2) Glyph Mode (64x40 text)
                                ;          3) 128x80 x 16-Color
GFX_AUX equ     $1802   ; (Byte) gfx auxillary/emulation flags:
                                ;      bit 7: 1:fullscreen / 0:windowed
                                ;      bit 6: reserved
                                ;      bit 5: reserved
                                ;      bit 4: reserved
                                ;      bit 3: reserved
                                ;      bit 0-2: monitor display index (0-7)
GFX_TIMING_W    equ     $1803   ; (Word) horizontal timing
GFX_TIMING_H    equ     $1805   ; (Word) vertical timing
GFX_PAL_INDX    equ     $1807   ; (Byte) gfx palette index (0-15)
GFX_PAL_DATA    equ     $1808   ; (Word) gfx palette color bits RGBA4444

;  Paged Foreground Graphics Mode Hardware Registers:
GFX_FG_BEGIN    equ     $180a   ; start of paged foreground gfxmode registers
GFX_FG_WDTH     equ     $180a   ; (Byte) Foreground Unit Width-1
GFX_FG_HGHT     equ     $180b   ; (Byte) Foreground Unit Height-1
GFX_FONT_IDX    equ     $180c   ; (Byte) Font Glyph Index
GFX_FONT_DAT    equ     $180d   ; (8-Bytes) Font Glyph Data Buffer
GFX_FG_END      equ     $1814   ; end of paged foreground gfxmode registers

;  Paged Background Graphics Mode Hardware Registers:
GFX_BG_BEGIN    equ     $1815   ; start of paged background gfxmode registers
GFX_EXT_ADDR    equ     $1815   ; (Word) 20K extended graphics addresses
GFX_EXT_DATA    equ     $1817   ; (Byte) 20K extended graphics RAM data
GFX_BG_END      equ     $1817   ; end of paged background gfxmode registers

;  Mouse Cursor Hardware Registers:
CSR_BEGIN       equ     $1818   ; start of mouse cursor hardware registers
CSR_XPOS        equ     $1818   ; (Word) horizontal mouse cursor coordinate
CSR_YPOS        equ     $181a   ; (Word) vertical mouse cursor coordinate
CSR_XOFS        equ     $181c   ; (Byte) horizontal mouse cursor offset
CSR_YOFS        equ     $181d   ; (Byte) vertical mouse cursor offset
CSR_SIZE        equ     $181e   ; (Byte) cursor size (0-15) 0:off
CSR_SCROLL      equ     $181f   ; (Signed) MouseWheel Scroll: -1, 0, 1
CSR_FLAGS       equ     $1820   ; (Byte) mouse button flags:
                                ;      bits 0-5: button states
                                ;      bits 6-7: number of clicks
CSR_PAL_INDX    equ     $1821   ; (Byte) mouse cursor color palette index (0-15)
CSR_PAL_DATA    equ     $1822   ; (Word) mouse cursor color palette data RGBA4444
CSR_BMP_INDX    equ     $1824   ; (Byte) mouse cursor bitmap pixel offset
CSR_BMP_DATA    equ     $1825   ; (Byte) mouse cursor bitmap pixel index color
CSR_END equ     $1825   ; end of mouse cursor hardware registers

;  Debugger Hardware Registers:
DBG_BEGIN       equ     $1826   ; Start of Debugger Hardware Registers
DBG_BRK_ADDR    equ     $1826   ; (Word) Address of current breakpoint
DBG_FLAGS       equ     $1828   ; (Byte) Debug Specific Hardware Flags
                                ;      bit 7: Debug Enable
                                ;      bit 6: Single Step Enable
                                ;      bit 5: clear all breakpoints
                                ;      bit 4: Toggle Breakpoint at DEBUG_BRK_ADDRESS
                                ;      bit 3: FIRQ  (on low to high edge)
                                ;      bit 2: IRQ   (on low to high edge)
                                ;      bit 1: NMI   (on low to high edge)
                                ;      bit 0: RESET (on low to high edge)
DBG_END equ     $1828   ; End of the Debugger Hardware Registers


GFX_END equ     $1829   ; end of the GFX Hardware Registers

;  File I/O Hardware Registers:
FIO_BEGIN       equ     $1829   ; start of file i/o hardware registers
FIO_ERR_FLAGS   equ     $1829   ; (Byte) file i/o system flags:
                                ;      bit 7:   file not found
                                ;      bit 6:  end of file
                                ;      bit 5:   buffer overrun
                                ;      bit 4: wrong file type
                                ;      bit 3: directory not found
                                ;      bit 0-2: not yet assigned
FIO_COMMAND     equ     $182a   ; (Byte) OnWrite - command to execute
                                ;      $00 = Reset/Null
                                ;      $01 = Open/Create Binary File for Reading
                                ;      $02 = Open/Create Binary File for Writing
                                ;      $03 = Open/Create Binary File for Append
                                ;      $04 = Close File
                                ;      $05 = Read Byte
                                ;      $06 = Write Byte
                                ;      $07 = Load Hex Format File
                                ;      $08 = Write Hex Format Line
                                ;      $09 = Get File Length (FIO_BFRLEN = file length
                                ;      $0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)
                                ;      $0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN)
                                ;      $0C = (not yet designed) List Directory
                                ;      $0D = Make Directory
                                ;      $0E = Change Directory
                                ;      $0F = Rename Directory
                                ;      $10 = Remove Directory
                                ;      $11 = Delete File
                                ;      $12 = Rename file
                                ;      $13 = Copy File
                                ;      $14 = Seek Start
                                ;      $15 = Seek Current
                                ;      $16 = Seek End
                                ;      $17 = SYSTEM: Shutdown
FIO_HANDLE      equ     $182b   ; (Byte) file handle or ZERO
FIO_BFROFS      equ     $182c   ; (Word) start of I/O buffer
FIO_BFRLEN      equ     $182d   ; (Word) length of I/O buffer
FIO_SEEKOFS     equ     $182f   ; (Word) seek offset
FIO_RET_COUNT   equ     $1831   ; (Byte) number of return entries
FIO_RET_INDEX   equ     $1832   ; (Byte) command return index
FIO_RET_BUFFER  equ     $1833   ; (Char Array 256) paged return buffer
FIO_FILEPATH    equ     $1933   ; (Char Array 256) file path and argument buffer
FIO_END equ     $1a33   ; end of file i/o hardware registers

;  Keyboard Hardware Registers:
KEY_BEGIN       equ     $1a34   ; start of keyboard hardware registers
CHAR_Q_LEN      equ     $1a34   ; (char) # of characters waiting in queue       (Read Only)
CHAR_SCAN       equ     $1a35   ; read next character in queue       (not popped when read)
CHAR_POP        equ     $1a36   ; (char) next character waiting in queue (popped when read)
XKEY_BUFFER     equ     $1a37   ; (128 bits) 16 bytes for XK_KEY data buffer    (Read Only)
EDT_BFR_CSR     equ     $1a47   ; (Byte) cursor position within edit buffer    (Read/Write)
EDT_BUFFER      equ     $1a48   ; (256 Bytes) line editing character buffer    (Read/Write)
KEY_END equ     $1b48   ; end of keyboard hardware registers

;  Reserved Hardware:
RESERVED_HDW    equ     $1b49   ; Reserved 1202 bytes ($1B49 - $1FFB)

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



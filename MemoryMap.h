/**** MemoryMap.h ***************************************
 *
 * Provides the memory map symbols for both/either
 * the C++ "Hardware" or 6809 ASM "Softere" systems.
 *
 * Copyright (C) 2023 by Jay Faries
 ************************************/

#pragma once
#ifndef __MEMORY_MAP__
#define __MEMORY_MAP__

#include "types.h"
#include <string>
#include <vector>

class MemoryMap
{
public:

	MemoryMap();
	~MemoryMap() { v_mem.clear(); }

	struct MemNode {
		Word address;
		std::string symbol;
		std::string comment;
	};

	void push(MemNode mem_node) { v_mem.push_back(mem_node); }
	Word start();				// begin mapping memory
	Word end(Word offset);		// end mapping memory

private:
	std::vector<MemNode> v_mem;

};


enum MEMMAP
{
//  **********************************************
//  * Allocated 64k Memory Mapped System Symbols *
//  **********************************************


//  SOFTWARE Interrupt Vectors:
    SOFT_RESET = 0x0000,        // Software RESET Vector
      SOFT_NMI = 0x0002,        // Software NMI Vector
      SOFT_SWI = 0x0004,        // Software SWI Vector
      SOFT_IRQ = 0x0006,        // Software IRQ Vector
     SOFT_FIRQ = 0x0008,        // Software FIRQ Vector
     SOFT_SWI2 = 0x000a,        // Software SWI2 Vector
     SOFT_SWI3 = 0x000c,        // Software SWI3 Vector
    SOFT_RSRVD = 0x000e,        // Software Motorola Reserved Vector

//  Zero-Page Kernal Variables:
   SYSTEM_VARS = 0x0010,        // start kernal vectors and variables
     KVEC_EXEC = 0x0010,        // KERNEL Vector:  Exec
      KVEC_CLS = 0x0012,        // KERNEL Vector:  Clear Text Screen
  KVEC_CHAROUT = 0x0014,        // KERNEL Vector:  Character Out
  KVEC_SHUTDOWN = 0x0016,       // KERNEL Vector:  System Shutdown

//  Stack Frames:
     U_STK_BTM = 0x0100,        // 256 bytes default user stack space
     U_STK_TOP = 0x0200,        // User Stack initial address
     S_STK_BTM = 0x0200,        // 512 bytes default system stack space
     S_STK_TOP = 0x0400,        // System Stack initial address

//  Video Buffer Memory (target = $0400):
   VIDEO_START = 0x0400,        // Start of 5KB Video Buffer Memory
     VIDEO_END = 0x17ff,        // Last Byte of 5KB Video Buffer Memory

//  Graphics Hardware Registers:
     GFX_BEGIN = 0x1800,        // start of graphics hardware registers
   GFX_CLK_DIV = 0x1800,        // (Byte) 60 hz Clock Divider:
                                //      bit 7: 0.46875 hz
                                //      bit 6: 0.9375 hz
                                //      bit 5: 1.875 hz
                                //      bit 4: 3.75 hz
                                //      bit 3: 7.5 hz
                                //      bit 2: 15.0 hz
                                //      bit 1: 30.0 hz
                                //      bit 0: 60.0 hz
     GFX_FLAGS = 0x1801,        // (Byte) gfx system flags:
                                //      bit 7: VSYNC
                                //      bit 6: backbuffer enable
                                //      bit 5: swap backbuffers (on write)
                                //      bit 4: reserved
                                //      bits 2-3 = 'Background' graphics modes (20KB buffer)
                                //          0) NONE (forced black background)
                                //          1) Tiled 16x16 mode
                                //          2) Overscan Tile 16x16 mode
                                //          3) 128x80 x 256-Colors
                                //      bits 0-1 = 'Foreground' graphics modes (5KB buffer)
                                //          0) 256x160 x 2-Color (with disable flag)
                                //          1) Glyph Mode (32x20 text)
                                //          2) Glyph Mode (64x40 text)
                                //          3) 128x80 x 16-Color
       GFX_AUX = 0x1802,        // (Byte) gfx auxillary/emulation flags:
                                //      bit 7: 1:fullscreen / 0:windowed
                                //      bit 6: reserved
                                //      bit 5: reserved
                                //      bit 4: reserved
                                //      bit 3: reserved
                                //      bit 0-2: monitor display index (0-7)
  GFX_TIMING_W = 0x1803,        // (Word) horizontal timing
  GFX_TIMING_H = 0x1805,        // (Word) vertical timing
  GFX_PAL_INDX = 0x1807,        // (Byte) gfx palette index (0-255)
  GFX_PAL_DATA = 0x1808,        // (Word) gfx palette color bits RGBA4444

//  Paged Foreground Graphics Mode Hardware Registers:
  GFX_FG_BEGIN = 0x180a,        // start of paged foreground gfxmode registers
   GFX_FG_WDTH = 0x180a,        // (Byte) Foreground Unit Width-1
   GFX_FG_HGHT = 0x180b,        // (Byte) Foreground Unit Height-1
  GFX_FONT_IDX = 0x180c,        // (Byte) Font Glyph Index
  GFX_FONT_DAT = 0x180d,        // (8-Bytes) Font Glyph Data Buffer
    GFX_FG_END = 0x1814,        // end of paged foreground gfxmode registers

//  Paged Background Graphics Mode Hardware Registers:
  GFX_BG_BEGIN = 0x1815,        // start of paged background gfxmode registers
  GFX_EXT_ADDR = 0x1815,        // (Word) 20K extended graphics addresses
  GFX_EXT_DATA = 0x1817,        // (Byte) 20K extended graphics RAM data
   GFX_BG_ARG1 = 0x1818,        // (Byte) first read/write argument for the blit commands
    GFX_BG_CMD = 0x1819,        // (Byte) Blit Commands on the indexed bitmap buffer
                                //      0x01: Clear Screen     (with color index in GFX_BG_ARG1)
                                //      0x02: unassigned
                                //      0x03: unassigned
                                //      0x04: Scroll Left      (by pixels x GFX_BG_ARG1)
                                //      0x05: Scroll Right     (by pixels x GFX_BG_ARG1)
                                //      0x06: Scroll Up        (by pixels x GFX_BG_ARG1)
                                //      0x07: Scroll Down      (by pixels x GFX_BG_ARG1)
                                //      0x08: Copy Buffer      (From Active to Inactive)
    GFX_BG_END = 0x1819,        // end of paged background gfxmode registers

//  Mouse Cursor Hardware Registers:
     CSR_BEGIN = 0x181a,        // start of mouse cursor hardware registers
      CSR_XPOS = 0x181a,        // (Word) horizontal mouse cursor coordinate
      CSR_YPOS = 0x181c,        // (Word) vertical mouse cursor coordinate
      CSR_XOFS = 0x181e,        // (Byte) horizontal mouse cursor offset
      CSR_YOFS = 0x181f,        // (Byte) vertical mouse cursor offset
      CSR_SIZE = 0x1820,        // (Byte) cursor size (0-15) 0:off
    CSR_SCROLL = 0x1821,        // (Signed) MouseWheel Scroll: -1, 0, 1
     CSR_FLAGS = 0x1822,        // (Byte) mouse button flags:
                                //      bits 0-5: button states
                                //      bits 6-7: number of clicks
  CSR_PAL_INDX = 0x1823,        // (Byte) mouse cursor color palette index (0-15)
  CSR_PAL_DATA = 0x1824,        // (Word) mouse cursor color palette data RGBA4444
  CSR_BMP_INDX = 0x1826,        // (Byte) mouse cursor bitmap pixel offset
  CSR_BMP_DATA = 0x1827,        // (Byte) mouse cursor bitmap pixel index color
       CSR_END = 0x1827,        // end of mouse cursor hardware registers

//  Debugger Hardware Registers:
     DBG_BEGIN = 0x1828,        // Start of Debugger Hardware Registers
  DBG_BRK_ADDR = 0x1828,        // (Word) Address of current breakpoint
     DBG_FLAGS = 0x182a,        // (Byte) Debug Specific Hardware Flags
                                //      bit 7: Debug Enable
                                //      bit 6: Single Step Enable
                                //      bit 5: clear all breakpoints
                                //      bit 4: Toggle Breakpoint at DEBUG_BRK_ADDRESS
                                //      bit 3: FIRQ  (on low to high edge)
                                //      bit 2: IRQ   (on low to high edge)
                                //      bit 1: NMI   (on low to high edge)
                                //      bit 0: RESET (on low to high edge)
       DBG_END = 0x182a,        // End of the Debugger Hardware Registers

//  Sprite Hardware Registers:
     SPR_BEGIN = 0x182b,        // Start of Sprite Hardware Registers

//  Sprite Flag Registers:
   SPR_DIS_ENA = 0x182b,        // (4-Bytes) sprite Enable Bits. 1 bit per sprite
   SPR_COL_ENA = 0x182f,        // (4-Bytes) sprite collision enable. 1 bit per sprite
   SPR_COL_TYP = 0x1833,        // (4-Bytes) sprite collision type (0:hitbox, 1:pixel perfect)

//  Sprite Palette Registers:
   SPR_PAL_IDX = 0x1837,        // (Byte) color palette index
   SPR_PAL_DAT = 0x1838,        // (Word) indexed sprite palette entry color bits RGBA4444

//  Sprite Indexed Registers:
   SPR_COL_DET = 0x183a,        // (4-Bytes) Collision detection bits. One bit per colliding sprite.
     SPR_H_POS = 0x183e,        // (Sint16) signed 16-bit integer
     SPR_V_POS = 0x1840,        // (Sint16) signed 16-bit integer
     SPR_X_OFS = 0x1842,        // (Sint8) signed 8-bit integer horizontal display offset
     SPR_Y_OFS = 0x1843,        // (Sint8) signed 8-bit integer vertical display offset
      SPR_PRIO = 0x1844,        // (Byte) Sprite Display Priority:
                                //      0) displays directly behind all foreground modes
                                //      1) displays infront of Glyph32 layer 0 but all other foreground modes
                                //      2) displays infront of Glyph32 layer 1 but all other foreground modes
                                //      3) displays infront of Glyph32 layer 2 but all other foreground modes
                                //      4) displays infront of Glyph32 layer 3 but all other foreground modes
                                //      5) displays infront of Debug layer, but behind the mouse cursor
                                //      6) displays infront of Mouse Cursor layer (in index order)
                                //      7) displays in sprite order

//  Sprite Indexed Bitmap Pixel Data:
   SPR_BMP_IDX = 0x1845,        // (Byte) Sprite pixel offset (Y*16+X)
   SPR_BMP_DAT = 0x1846,        // (Byte) Sprite color palette index data

//  End of Sprite Hardware Registers
       SPR_END = 0x1846,        // End of the Sprite Hardware Registers

//  End ofthe GFX Hardware Registers
       GFX_END = 0x1847,        // end of the GFX Hardware Registers

//  File I/O Hardware Registers:
     FIO_BEGIN = 0x1847,        // start of file i/o hardware registers
  FIO_ERR_FLAGS = 0x1847,       // (Byte) file i/o system flags:
                                //      bit 7:  file not found
                                //      bit 6:  end of file
                                //      bit 5:  buffer overrun
                                //      bit 4: wrong file type
                                //      bit 3: directory not found
                                //      bit 2: too many file handles
                                //      bit 1: incorrect file handle
                                //      bit 0: not yet assigned
   FIO_COMMAND = 0x1848,        // (Byte) OnWrite - command to execute
                                //      $00 = Reset/Null
                                //      $01 = Open/Create Binary File for Reading
                                //      $02 = Open/Create Binary File for Writing
                                //      $03 = Open/Create Binary File for Append
                                //      $04 = Close File
                                //      $05 = Read Byte
                                //      $06 = Write Byte
                                //      $07 = Load Hex Format File
                                //      $08 = Write Hex Format Line
                                //      $09 = Get File Length (FIO_BFRLEN = file length
                                //      $0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)
                                //      $0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN)
                                //      $0C = (not yet designed) List Directory
                                //      $0D = Make Directory
                                //      $0E = Change Directory
                                //      $0F = Rename Directory
                                //      $10 = Remove Directory
                                //      $11 = Delete File
                                //      $12 = Rename file
                                //      $13 = Copy File
                                //      $14 = Seek Start
                                //      $15 = Seek Current
                                //      $16 = Seek End
                                //      $17 = SYSTEM: Shutdown
                                //      $18 = SYSTEM: Load Compilation Date
    FIO_HANDLE = 0x1849,        // (Byte) file handle or ZERO
    FIO_BFROFS = 0x184a,        // (Word) start of I/O buffer
    FIO_BFRLEN = 0x184b,        // (Word) length of I/O buffer
    FIO_IODATA = 0x184d,        // (Byte) input / output character
  FIO_RET_COUNT = 0x184e,       // (Byte) number of return entries
  FIO_RET_INDEX = 0x184f,       // (Byte) command return index
  FIO_RET_BUFFER = 0x1850,      // (Char Array 256) paged return buffer
  FIO_FILEPATH = 0x1950,        // (Char Array 256) file path and argument buffer
       FIO_END = 0x1a50,        // end of file i/o hardware registers

//  Keyboard Hardware Registers:
     KEY_BEGIN = 0x1a51,        // start of keyboard hardware registers
    CHAR_Q_LEN = 0x1a51,        // (char) # of characters waiting in queue       (Read Only)
     CHAR_SCAN = 0x1a52,        // read next character in queue       (not popped when read)
      CHAR_POP = 0x1a53,        // (char) next character waiting in queue (popped when read)
   XKEY_BUFFER = 0x1a54,        // (128 bits) 16 bytes for XK_KEY data buffer    (Read Only)
   EDT_BFR_CSR = 0x1a64,        // (Byte) cursor position within edit buffer    (Read/Write)
    EDT_BUFFER = 0x1a65,        // (256 Bytes) line editing character buffer    (Read/Write)
       KEY_END = 0x1b65,        // end of keyboard hardware registers

//  Gamepad Hardware Registers:
    JOYS_BEGIN = 0x1b66,        // start of gamepad hardware registers
    JOYS_1_BTN = 0x1b66,        // (Word) button bits: room for up to 16 buttons  (realtime)
   JOYS_1_DBND = 0x1b68,        // (Byte) PAD 1 analog deadband; default is 5   (read/write)
    JOYS_1_LTX = 0x1b69,        // (char) PAD 1 LThumb-X position (-128 _ +127)   (realtime)
    JOYS_1_LTY = 0x1b6a,        // (char) PAD 1 LThumb-Y position (-128 _ +127)   (realtime)
    JOYS_1_RTX = 0x1b6b,        // (char) PAD 1 RThumb-X position (-128 _ +127)   (realtime)
    JOYS_1_RTY = 0x1b6c,        // (char) PAD 1 RThumb-Y position (-128 _ +127)   (realtime)
     JOYS_1_Z1 = 0x1b6d,        // (char) PAD 1 left trigger        (0 - 127)     (realtime)
     JOYS_1_Z2 = 0x1b6e,        // (char) PAD 1 right trigger       (0 - 127)     (realtime)

    JOYS_2_BTN = 0x1b6f,        // (Word) button bits: room for up to 16 buttons  (realtime)
   JOYS_2_DBND = 0x1b71,        // (Byte) PAD 2 analog deadband; default is 5   (read/write)
    JOYS_2_LTX = 0x1b72,        // (char) PAD 2 LThumb-X position (-128 _ +127)   (realtime)
    JOYS_2_LTY = 0x1b73,        // (char) PAD 2 LThumb-Y position (-128 _ +127)   (realtime)
    JOYS_2_RTX = 0x1b74,        // (char) PAD 2 RThumb-X position (-128 _ +127)   (realtime)
    JOYS_2_RTY = 0x1b75,        // (char) PAD 2 RThumb-Y position (-128 _ +127)   (realtime)
     JOYS_2_Z1 = 0x1b76,        // (char) PAD 2 left trigger        (0 - 127)     (realtime)
     JOYS_2_Z2 = 0x1b77,        // (char) PAD 2 right trigger       (0 - 127)     (realtime)
      JOYS_END = 0x1b78,        // end of gamepad hardware registers

//  Reserved Hardware:
  RESERVED_HDW = 0x1b79,        // Reserved 1154 bytes ($1B79 - $1FFB)

//  Memory Bank Selects (external 2MB QSPI ISSI 16Mbit SerialRAM):
//  https://www.mouser.com/ProductDetail/ISSI/IS66WVS2M8BLL-104NLI?qs=doiCPypUmgFx786bHGqGiQ%3D%3D
  RAMBANK_SEL_1 = 0x1ffe,       // (Byte)Indexes 256 x 8kb banks
  RAMBANK_SEL_2 = 0x1fff,       // (Byte)Indexes 256 x 8kb banks

//  Standard Usable (from FAST static 32KB) RAM:
     RAM_START = 0x2000,        // Begin System RAM (32k)
       RAM_END = 0x9fff,        // End System RAM

//  Switchable RAM Banks (from SLOW external serial 16MB RAM chip):
    RAM_BANK_1 = 0xa000,        // switched 8KB ram bank 1
    RAM_BANK_2 = 0xc000,        // switched 8KB ram bank 2

//  Bios Kernal ROM:
      BIOS_ROM = 0xe000,        // Begin BIOS Kernal ROM (8KB)

//  Hardware Interrupt Vectors:
    HARD_RSRVD = 0xfff0,        // Motorola RESERVED Hardware Interrupt Vector
     HARD_SWI3 = 0xfff2,        // SWI3 Hardware Interrupt Vector
     HARD_SWI2 = 0xfff4,        // SWI2 Hardware Interrupt Vector
     HARD_FIRQ = 0xfff6,        // FIRQ Hardware Interrupt Vector
      HARD_IRQ = 0xfff8,        // IRQ Hardware Interrupt Vector
      HARD_SWI = 0xfffa,        // SWI / SYS Hardware Interrupt Vector
      HARD_NMI = 0xfffc,        // NMI Hardware Interrupt Vector
    HARD_RESET = 0xfffe,        // RESET Hardware Interrupt Vector
};

#endif // __MEMORY_MAP__



/****** MEMORY NOTES:  *************************************************

    The use of 16MByte external buffer may be a bit overkill. Reconsider using
    a single 16Mb SerialRAM chip for 2MBytes of total paged memory programmatically
    clocked at full single thread ARM M0+ speed. SPI mode would work, but QPI mode would be, 
    theoretically faster if the PICO can handle it.

    Considering the use of:  IS66WVS2M8BLL-104NLI from Mouser...
        see: https://www.mouser.com/ProductDetail/ISSI/IS66WVS2M8BLL-104NLI?qs=doiCPypUmgFx786bHGqGiQ%3D%3D
    2 megabytes of volitile Serial Static RAM. 

    Remeber, the PICO includes 2MB on-board flash memory. Some (unknown at this point) will be
    used to load the System onto the PICO at startup. However, the bulk of the flash memory
    will be available as "persistent" banked memory.

    IDEA:  
        - RAM_BANK_1 = 2 megabytes of Static SerialRAM via QSPI to the ISSI SRAM chip
        - RAM_BANK_2 = up to 2 megabytes of remaining flash RAM from both PICO boards. This should
                leave plenty of flash storage for the required OS files.

        In other words, RAM_BANK_1 would be the volitile SRAM and ...
        RAM_BANK_2 would be a persistent and non-volitile RAM container using flash ram from 
        both PICOs. 
    
    For now, just map 2 megabytes into each bank.
    
    
    *****************************************************************/
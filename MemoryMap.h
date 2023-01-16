// * MemoryMap.h ***************************************
// *
// * Provides the memory map symbols for both/either
// * the C++ "Hardware" or 6809 ASM "Softere" systems.
// ************************************

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

//  Stack Frames:
     U_STK_BTM = 0x0100,        // 256 bytes default user stack space
     U_STK_TOP = 0x0200,        // User Stack initial address
     S_STK_BTM = 0x0200,        // 512 bytes default system stack space
     S_STK_TOP = 0x0400,        // System Stack initial address

//  Video Buffer Memory (target = $0400):
   VIDEO_START = 0x0400,        // Start of Video Buffer Memory
     VIDEO_END = 0x17ff,        // Last Byte of Video Buffer Memory

//  Graphics Hardware Registers:
     GFX_FLAGS = 0x1800,        // (Byte) gfx system flags:
                                //      bit 7: VSYNC
                                //      bit 6: backbuffer enable
                                //      bit 5: swap backbuffers (on write)
                                //      bit 4: debug enable
                                //      bits 2-3 = 'Background' graphics mode (40KB buffer)
                                //          0) NONE (forced black background)
                                //          1) Tiled 16x16 mode
                                //          2) 256x160 x 64-Colors
                                //          3) 512x320 x 4-Color
                                //      bits 0-1 = 'Foreground' graphics mode (5KB buffer)
                                //          0) 256x160 x 2-Color (with disable flag)
                                //          1) Glyph Mode (32x20 text)
                                //          2) Glyph Mode (64x40 text)
                                //          3) 128x80 x 16-Color
       GFX_AUX = 0x1801,        // (Byte) gfx auxillary/emulation flags:
                                //      bit 7: 1:fullscreen / 0:windowed
                                //      bit 6: reserved
                                //      bit 5: reserved
                                //      bit 4: reserved
                                //      bit 3: reserved
                                //      bit 0-2: monitor display index (0-7)
  TIMING_WIDTH = 0x1802,        // (Word) timing width
  TIMING_HEIGHT = 0x1804,       // (Word) timing height
  GFX_PAL_INDX = 0x1806,        // (Byte) gfx palette index (0-15)
  GFX_PAL_DATA = 0x1807,        // (Byte) gfx palette color bits r4g4b4a4

//  Paged Graphics Mode Hardware Registers:
  GFX_PG_BEGIN = 0x1808,        // start of paged gfxmode registers
  GFX_EXT_ADDR = 0x1808,        // (Word) 64K extended graphics addresses
  GFX_EXT_DATA = 0x180a,        // (Byte) 64K extended graphics RAM data
    GFX_PG_END = 0x180a,        // end of paged gfxmode registers

//  Mouse Cursor Hardware Registers:
      CSR_XPOS = 0x180b,        // (Word) horizontal mouse cursor coordinate
      CSR_YPOS = 0x180d,        // (Word) vertical mouse cursor coordinate
      CSR_XOFS = 0x180f,        // (Byte) horizontal mouse cursor offset
      CSR_YOFS = 0x1810,        // (Byte) vertical mouse cursor offset
      CSR_SIZE = 0x1811,        // (Byte) cursor size (0-15) 0:off
    CSR_SCROLL = 0x1812,        // (Signed) MouseWheel Scroll: -1, 0, 1
     CSR_FLAGS = 0x1813,        // (Byte) mouse button flags:
                                //      bits 0-5: button states
                                //      bits 6-7: number of clicks
  CSR_PAL_INDX = 0x1814,        // (Byte) mouse cursor color palette index (0-15)
  CSR_PAL_DATA = 0x1815,        // (Byte) mouse cursor color palette data RRGGBBAA
  CSR_BMP_INDX = 0x1816,        // (Byte) mouse cursor bitmap pixel offset
  CSR_BMP_DATA = 0x1817,        // (Byte) mouse cursor bitmap pixel color

       GFX_END = 0x1817,        // end of the GFX Hardware Registers

  RESERVED_HDW = 0x1818,        // Reserved 2019 bytes ($1818 - $1FFB)

//  Memory Bank Selects (16MB):
  RAMBANK_SEL_1 = 0x1ffc,       // (Word)Indexes 65536 x 8kb banks
  RAMBANK_SEL_2 = 0x1ffe,       // (Word)Indexes 65536 x 8kb banks

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


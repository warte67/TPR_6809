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
//      bit 7: fullscreen
//      bit 6: vsync
//      bit 5: unassigned
//      bit 4: unassigned
//      bit 3: unassigned
//      bit 0-2: display monitor (0-7)
     SCR_WIDTH = 0x1801,        // (Word) timing width
    SCR_HEIGHT = 0x1803,        // (Word) timing height
     PIX_WIDTH = 0x1805,        // (Word) pixel width
    PIX_HEIGHT = 0x1807,        // (Word) pixel height
  RESERVED_HDW = 0x2000,        // Reserved ($2000-$1FFB)

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


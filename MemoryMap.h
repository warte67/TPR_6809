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

private:

	struct MemNode {
		Word address;
		std::string symbol;
		std::string comment;
	};

	std::vector<MemNode> v_mem;

};

/******************************************************
* MEMORY MAP SYMBOL DEFINITIONS (copied from console) *
*******************************************************/

enum MEMMAP
{

};


#endif // __MEMORY_MAP__


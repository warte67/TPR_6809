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

/******************************************************
* MEMORY MAP SYMBOL DEFINITIONS (copied from console) *
*******************************************************/

enum MEMMAP
{

};


#endif // __MEMORY_MAP__


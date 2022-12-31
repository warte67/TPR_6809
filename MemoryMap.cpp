// * MemoryMap.cpp ***************************************
// *
// * Provides the memory map symbols for both/either
// * the C++ "Hardware" or 6809 ASM "Softere" systems.
// ************************************

#include "MemoryMap.h"
#include "Bus.h"

MemoryMap::MemoryMap()
{
}

Word MemoryMap::start()
{
	auto hex = [](uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	bool bError = false;
	Word offset = 0;
	v_mem.push_back({ offset, "", "**********************************************" });
	v_mem.push_back({ offset, "", "* Allocated 64k Memory Mapped System Symbols *" });
	v_mem.push_back({ offset, "", "**********************************************" });
	v_mem.push_back({ offset, "", "" });

	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "SOFTWARE Interrupt Vectors:" });
	v_mem.push_back({ offset, "SOFT_RESET",	"Software RESET Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_NMI",	"Software NMI Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_SWI",	"Software SWI Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_IRQ",	"Software IRQ Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_FIRQ",	"Software FIRQ Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_SWI2",	"Software SWI2 Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_SWI3",	"Software SWI3 Vector" }); offset += 2;
	v_mem.push_back({ offset, "SOFT_RSRVD", "Software Motorola Reserved Vector" }); offset += 2;

	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Zero-Page Kernal Variables:" });
	v_mem.push_back({ offset, "SYSTEM_VARS", "start kernal vectors and variables" }); offset += 0x0f0;

	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Stack Frames:" });
	v_mem.push_back({ offset, "U_STK_BTM", "256 bytes default user stack space" }); offset += 0x100;
	v_mem.push_back({ offset, "U_STK_TOP", "User Stack initial address" });
	v_mem.push_back({ offset, "S_STK_BTM", "512 bytes default system stack space" }); offset += 0x200;
	v_mem.push_back({ offset, "S_STK_TOP", "System Stack initial address" });

	// 5K video buffer ($0400 - $1800) 
	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Video Buffer Memory (target = $0400):" });
	v_mem.push_back({ offset, "VIDEO_START", "Start of Video Buffer Memory" }); offset += 0x1400 - 1;
	v_mem.push_back({ offset, "VIDEO_END", "Last Byte of Video Buffer Memory" }); offset += 1;

	return offset;
}

Word MemoryMap::end(Word offset)
{
	auto hex = [](uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	bool bError = false;

	// Hardware Register Upper Bounds Checking:
	Word _n = (0x2000 - 5);
	if (offset >= _n)
	{
		Bus::Err("Hardware register upper bounds limit exceeded!");
		bError = true;
	}

	// Reserved for future Hardware Expansion
	std::string _future_expansion = "Reserved ($" + hex(offset, 4) + "-$" + hex(_n, 4) + ")";
	v_mem.push_back({ offset, "RESERVED_HDW", _future_expansion });
	offset = 0x2000 - 4;

	// Memory Bank Select(s)
	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Memory Bank Selects (16MB):" });
	v_mem.push_back({ offset, "RAMBANK_SEL_1", "(Word)Indexes 65536 x 8kb banks" }); offset += 2;
	v_mem.push_back({ offset, "RAMBANK_SEL_2", "(Word)Indexes 65536 x 8kb banks" }); offset += 2;

	// Begin System Ram ($2000-AFFF)
	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Standard Usable (from FAST STATIC 32KB) RAM:" });
	v_mem.push_back({ offset, "RAM_START", "Begin System RAM (32k)" }); offset += 0x8000 - 1;
	v_mem.push_back({ offset, "RAM_END", "End System RAM" }); offset += 1;

	// Switchable Memory Bank(s)
	Word _size = 0xe000 - offset;
	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Switchable RAM Banks (from SLOW external serial 16MB RAM chip):" });
	v_mem.push_back({ offset, "RAM_BANK_1", "switched 8KB ram bank 1" }); offset += _size / 2;
	v_mem.push_back({ offset, "RAM_BANK_2", "switched 8KB ram bank 2" }); offset += _size / 2;

	// Bios Kernal ROM:
	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Bios Kernal ROM:" });
	v_mem.push_back({ offset, "BIOS_ROM", "Begin BIOS Kernal ROM (8KB)" }); offset += 0x2000;

	// verify memory map
	if (offset != 0)
	{
		printf("offset = $%04x\n", offset);
		Bus::Err("Invalid Memory Map. Total Allocated Bytes is NOT 64k.");
		bError = true;
	}

	// Hardware Interrupt Vectors:
	offset = 0xfff0;
	v_mem.push_back({ offset, "", "" });
	v_mem.push_back({ offset, "", "Hardware Interrupt Vectors:" });
	v_mem.push_back({ offset, "HARD_RSRVD", "Motorola RESERVED Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_SWI3", "SWI3 Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_SWI2", "SWI2 Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_FIRQ", "FIRQ Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_IRQ", "IRQ Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_SWI", "SWI / SYS Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_NMI", "NMI Hardware Interrupt Vector" }); offset += 2;
	v_mem.push_back({ offset, "HARD_RESET", "RESET Hardware Interrupt Vector" }); offset += 2;


	if (bError)
	{
		printf("\n\n");
		return offset;
	}

	if (MEMORY_MAP_CPP)
	{
		// C++ Style enum
		std::string line_comment_character = "// ";
		printf("enum MEMMAP\n{\n");
		for (auto& a : v_mem)
		{
			if (a.symbol != "")
			{
				printf("  %12s = 0x%04x,\t%s%s\n",
					a.symbol.c_str(),
					a.address,
					line_comment_character.c_str(),
					a.comment.c_str());
			}
			else
			{
				if (a.comment == "")
					printf("\n");
				else
					printf("%s**** %s ****\n", line_comment_character.c_str(), a.comment.c_str());
			}
		}
		printf("};");
	}
	else
	{
		// ASM Style enum
		std::string line_comment_character = "; ";
		for (auto& a : v_mem)
		{
			if (a.symbol != "")
			{
				printf("%12s\tequ\t$%04x\t%s%s\n",
					a.symbol.c_str(),
					a.address,
					line_comment_character.c_str(),
					a.comment.c_str());
			}
			else
				if (a.comment == "")
					printf("\n");
				else
					printf("\t%s**** %s ****\n", line_comment_character.c_str(), a.comment.c_str());
		}
	}
	printf("\n\n");


	return offset;
}
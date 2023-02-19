/* Math.h ***************************************
 *
 *   Implements IEEE 754 floating point math processing.
 *		Both half and single precision should be implemented.
 *		Single precision comes from standard C++, but
 *		Half precision comes from the 16-bit "Half-precision
 *		floating-point library" by Christian Rau under the
 *		MIT liscense. (https://half.sourceforge.net/index.html)
 *
 * Copyright (C) 2023 by Jay Faries
 ************************************/

#include "types.h"
#include "Bus.h"
#include "Memory.h"
#include "half.hpp"
	using namespace half_float;
	using namespace literal;

#include "Math.h"


Byte Math::OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead)
{
	Bus* bus = Bus::getInstance();
	Math* ptrMath = dynamic_cast<Math*>(reg);
	if (ptrMath)
	{
		// if (ofs >= MATH_BEGIN && ofs <= MATH_END)
		{
			if (bWasRead)
			{
			}
		}
	}
	return data;
}

Math* Math::Assign_Math(MemoryMap* memmap, DWord& offset)
{
	Bus* bus = Bus::getInstance();
	Math* ret = nullptr;
	// attach a FileIO device:
	int fSize = Math::MapDevice(memmap, offset);
	ret = new Math(offset, fSize);
	ret->bus = bus;
	ret->memory = bus->m_memory;
	bus->AttachDevice(ret);		// bus->_devices.push_back(ret);
	bus->m_memory->ReassignReg(offset, ret, "MATH_HDW", fSize, Math::OnCallback);
	ret->Base(offset);
	ret->Size(fSize);
	bus->m_memory->NextAddress(offset + fSize);
	offset += fSize;
	return ret;
}


Word Math::MapDevice(MemoryMap* memmap, Word offset)
{
	int start_offset = offset;

	//memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Math Hardware Registers:					" }); offset += 0;
	memmap->push({ offset, "MATH_BEGIN",	"start of math hardware registers             " }); offset += 0;

	memmap->push({ offset, "MATH_RNDINT",	"(Word) 16-bit random integer                 " }); offset += 2;
	memmap->push({ offset, "MATH_RNDHALF",	"(Word) normalized half-precision float (0.0 - 1.0)   " }); offset += 2;

	memmap->push({ offset, "MATH_END",		"end of math hardware registers               " }); offset += 1;
	memmap->push({ offset, "", "" });

	return offset - start_offset;
}



/////////////////////////////////////////////////////////////////////////////////


Math::Math() : REG(0, 0)
{
	Device::_deviceName = "???Math???";
	// this constructore is removed early.
	// dont use it for initialization
}

Math::Math(Word offset, Word size) : REG(offset, size)
{
	Device::_deviceName = "MATH";
	bus = Bus::getInstance();
	bus->m_math = this;
	this->memory = bus->getMemoryPtr();
}

Math::~Math()
{
}


/////////////////////////////////////////////////////////////////////////////////


void Math::OnInitialize()
{
	printf("Math::OnInitialize()\n");
}

void Math::OnEvent(SDL_Event* evnt) 
{
	//printf("Math::OnEvent()\n");
}

void Math::OnCreate() 
{
	printf("Math::OnCreate()\n");
}

void Math::OnDestroy() 
{
	printf("Math::OnDestroy()\n");
}

void Math::OnUpdate(float fElapsedTime) 
{
	//printf("Math::OnUpdate()\n");
}

void Math::OnQuit()	
{
	printf("Math::OnQuit()\n");
}


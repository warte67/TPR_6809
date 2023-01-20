// * Keyboard.cpp ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico #2
// *		Dispatches Keyboard Events to PICO #1
// *		Emulates the Keyboard Hardware Registers
// ************************************

#include "types.h"
#include "Bus.h"
#include "Memory.h"
#include "Keyboard.h"

// memory-map callback function
Byte Keyboard::OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead)
{
	Bus* bus = Bus::getInstance();
	Keyboard* ptrKey = dynamic_cast<Keyboard*>(reg);
	if (ptrKey)
	{
		//if (ofs >= FIO_BEGIN && ofs <= FIO_END)
		{
			if (bWasRead)
			{	// READ FROM
				printf("Keyboard::OnCallback()->READ\n");
			}
			else
			{	// WRITE TO
				printf("Keyboard::OnCallback()->WRITE\n");

				bus->debug_write(ofs, data);
			}
		}
	}
	return data;
}


Keyboard* Keyboard::Assign_Keyboard(MemoryMap* memmap, DWord& offset)
{
	Bus* bus = Bus::getInstance();
	Keyboard* ret = nullptr;
	// attach a FileIO device:
	int fSize = Keyboard::MapDevice(memmap, offset);
	ret = new Keyboard(offset, fSize);
	ret->bus = bus;
	ret->memory = bus->m_memory;
	bus->AttachDevice(ret);		// bus->_devices.push_back(ret);
	bus->m_memory->ReassignReg(offset, ret, "KEYBOARD_HDW", fSize, Keyboard::OnCallback);

	ret->Base(offset);
	ret->Size(fSize);

	bus->m_memory->NextAddress(offset + fSize);

	offset += fSize;

	return ret;
}

Word Keyboard::MapDevice(MemoryMap* memmap, Word offset)
{
	int start_offset = offset;

	//memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Keyboard Hardware Registers:                   " }); offset += 0;
	memmap->push({ offset, "KEY_BEGIN", "start of keyboard hardware registers  " }); offset += 0;

	memmap->push({ offset, "KEY_TEMP1", "(Byte) temporary keyboard register     " }); offset += 1;
	memmap->push({ offset, "KEY_TEMP2", "(Byte) secondary keyboard register     " }); offset += 1;

	memmap->push({ offset, "KEY_END", "end of keyboard hardware registers     " }); offset += 1;
	memmap->push({ offset, "", "" });

	return offset - start_offset;
}

Keyboard::Keyboard() : REG(0, 0)
{
	Device::_deviceName = "???Keyboard???";
	// this constructore is removed early.
	// dont use it for initialization
}

Keyboard::Keyboard(Word offset, Word size) : REG(offset, size)
{
	Device::_deviceName = "KEYBOARD";
	bus = Bus::getInstance();
	bus->m_keyboard = this;
	this->memory = bus->getMemoryPtr();
}

Keyboard::~Keyboard()
{
}


void Keyboard::OnInitialize()
{
	Byte data = bus->read(KEY_TEMP1);
	printf("Before: %d\n", data);
	data++;
	bus->write(KEY_TEMP1, data);
	printf("After: %d\n", data);
}

void Keyboard::OnEvent(SDL_Event* evnt) {}
void Keyboard::OnCreate() {}
void Keyboard::OnDestroy() {}
void Keyboard::OnUpdate(float fElapsedTime) {}
void Keyboard::OnQuit() {}


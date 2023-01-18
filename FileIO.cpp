// * FileIO.cpp ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		Implementing File Input / Output
// *		with the onboad flash ram and
// *		the SDCARD
// ************************************

#include "types.h"
#include "Bus.h"
#include "Memory.h"
#include "FileIO.h"

// memory-map callback function
Byte FileIO::OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead)
{
	Bus* bus = Bus::getInstance();
	FileIO* ptrFile = dynamic_cast<FileIO*>(reg);
	if (ptrFile)
	{
		if (bWasRead)
		{	// READ FROM
			//printf("FileIO::OnCallback() =-->  READ\n");
		}
		else
		{	// WRITE TO
			//printf("FileIO::OnCallback() =-->  WRITE\n");

			bus->debug_write(ofs, data);
		}
	}
	return data; 
}

FileIO* FileIO::Assign_FileIO(MemoryMap* memmap, DWord& offset)
{
	Bus* bus = Bus::getInstance();
	FileIO* ret = nullptr;
	// attach a FileIO device:
	int fSize = FileIO::MapDevice(memmap, offset);
	ret = new FileIO(offset, fSize);
	ret->bus = bus;
	ret->memory = bus->m_memory;	
	bus->AttachDevice(ret);		// bus->_devices.push_back(ret);
	bus->m_memory->ReassignReg(offset, ret, "FILEIO_HDW", fSize, FileIO::OnCallback);
	offset += fSize;

	return ret;
}

Word FileIO::MapDevice(MemoryMap* memmap, Word offset) 
{ 
	int start_offset = offset;

	//memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "File I/O Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "FIO_BEGIN", "start of file i/o hardware registers" }); offset += 0;

	memmap->push({ offset, "FIO_FLAGS", "(Byte) file i/o system flags:" }); offset += 1;
	memmap->push({ offset, "", ">    bit 7: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 6: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 5: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 4: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 3: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 2: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 1: not yet assigned" }); offset += 0;
	memmap->push({ offset, "", ">    bit 0: not yet assigned" }); offset += 0;

	memmap->push({ offset, "FIO_END", "end of the file i/o Hardware Registers" });
	memmap->push({ offset, "", "" });

	return offset - start_offset; 
}

FileIO::FileIO() : REG(0, 0)
{
	Device::_deviceName = "???FileIO???";
	// this constructore is removed early.
	// dont use it for initialization
}

FileIO::FileIO(Word offset, Word size) : REG(offset, size) 
{
	Device::_deviceName = "FILEIO";
	bus = Bus::getInstance();
	bus->m_file = this;

	this->memory = bus->getMemoryPtr();
}

FileIO::~FileIO() 
{
}


void FileIO::OnInitialize() 
{
	//printf("FileIO::OnInitialize()\n");

	Byte d = bus->read(FIO_FLAGS);
	d++;
	bus->write(FIO_FLAGS, d);
	printf("Read(FIO_FLAGS): %d\n", bus->read(FIO_FLAGS));	
	d++;
	bus->write(FIO_FLAGS, d);
	printf("Read(FIO_FLAGS): %d\n", bus->read(FIO_FLAGS));	
	d++;
	bus->write(FIO_FLAGS, d);
	printf("Read(FIO_FLAGS): %d\n", bus->read(FIO_FLAGS));	
	d++;
	bus->write(FIO_FLAGS, d);
	printf("Read(FIO_FLAGS): %d\n", bus->read(FIO_FLAGS));
}

void FileIO::OnEvent(SDL_Event* evnt) {}
void FileIO::OnCreate() {}
void FileIO::OnDestroy() {}
void FileIO::OnUpdate(float fElapsedTime) {}
void FileIO::OnQuit() {}





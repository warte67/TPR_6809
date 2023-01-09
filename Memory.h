// * Memory.h ***************************************
// *
// * Implements the RAM, ROM, and REG (register) type
// *    memory devices.
// ************************************
#pragma once
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Device.h"

class BUS;
class RAM;
class ROM;
class REG;

//// class Memory ///////////////////////////////////

class Memory : public Device
{
	friend class Bus;

public:
	Memory();
	Memory(Word offset, Word size) : Device(offset, size) {
		_deviceName = "Memory";
		// bus = Bus::getInstance();
	}	
	virtual ~Memory();

	// map this devices hardware registers (TODO: move to class Memory)
	virtual Word MapDevice(MemoryMap* memmap, Word offset)  { return offset; }

	// abstract members
	virtual void OnInitialize() override;				// runs once after all devices are created
	virtual void OnEvent(SDL_Event* evnt) override;		// fires per SDL_Event
	virtual void OnCreate() override;					// fires when the object is created/recreated
	virtual void OnDestroy() override;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) override;	// fires each frame, for updates
	//virtual void OnRender() override;					// render the current frames texture
	virtual void OnQuit() override;						// fires on exit -- reverses OnInitialize()

	// virtual accessors

	DWord AssignRAM(std::string cDesc, DWord size);
	DWord AssignROM(std::string cDesc, Word size, const char* file);
	DWord AssignREG(std::string cDesc, Word size, Byte(*callback)(REG*, Word, Byte, bool));
	DWord ReassignReg(Word, REG*, std::string, Word, Byte(*callback)(REG*, Word, Byte, bool));
	REG* FindRegByName(std::string name);

	Byte read(Word offset);
	void write(Word offset, Byte data);
	Word read_word(Word offset);
	void write_word(Word offset, Word data);

	Byte debug_read(Word offset);
	void debug_write(Word offset, Byte data);
	Word debug_read_word(Word offset);
	void debug_write_word(Word offset, Word data);

protected:
	Bus* bus = nullptr;

	std::vector<Memory*> m_memBlocks;
	DWord nextAddress = 0; 
};




//// class RAM /////////////////////////////////////

class RAM : public Memory
{
	friend class Memory;

public:
	RAM(Word offset, Word size);
	virtual ~RAM();

public:		// virtual members
	virtual Byte read(Word offset) override;
	virtual void write(Word offset, Byte data) override;
};


//// class ROM /////////////////////////////////////

class ROM : public Memory
{
	friend class Memory;

public:
	ROM(Word offset, Word size);
	virtual ~ROM();

	virtual Byte read(Word offset) override;
	virtual void write(Word offset, Byte data) override;

	void load_hex(const char* filename, Word base);
	void setFilename(const char* filename) { fileName = filename; }

protected:
	std::string fileName;
};


//// class REG (hardware register)  /////////////////////

class REG : public Memory
{
	friend class Memory;

public:
	REG(Word offset, Word size, Byte(*callback)(REG*, Word, Byte, bool));
	REG(Word offset, Word size) : Memory(offset, size) {
		_deviceName = "Memory";
		//bus = Bus::getInstance();
	}

	//Byte read(Word offset) override {
	//	if (offset - base < size)
	//		return memory[(Word)(offset - base)];
	//	return 0xff;
	//}

	//void write(Word offset, Byte data) override {
	//	if (offset - base < size)
	//		memory[(Word)(offset - base)] = data;
	//}

	virtual ~REG();

	void RegisterCallback(Byte(*_callback)(REG* module, Word ofs, Byte data, bool bWasRead)) {
		callback = _callback;
	}

public:
	Byte(*callback)(REG* module, Word ofs, Byte data, bool bWasRead);
};



#endif // __MEMORY_H__


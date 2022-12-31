// * Memory.cpp ***************************************
// *
// * Implements the RAM, ROM, and REG (register) type
// *    memory devices.
// ************************************

#include "Bus.h"
#include "Device.h"
#include "Memory.h"

Memory::Memory() : Device("Memory") 
{
}
Memory::~Memory()
{
	for (auto& a : m_memBlocks)
		delete a;
}

//// ABSTRACT OVERRIDES //////////////////////////////

// runs once after all devices are created
void Memory::OnInitialize()
{
}

// fires per SDL_Event
void Memory::OnEvent(SDL_Event* evnt)
{
}

// fires when the object is created/recreated
void Memory::OnCreate()
{
}

// fires when the object is destroyed/lost focus
void Memory::OnDestroy()
{
}

// fires each frame, for updates
void Memory::OnUpdate(float fElapsedTime)
{
}

// render the current frames texture
void Memory::OnRender()
{
}

// fires on exit -- reverses OnInitialize()
void Memory::OnQuit()
{
}



//// VIRTUAL ACCESSORS //////////////////////////////

DWord Memory::AssignRAM(std::string cDesc, DWord size) {
	RAM* ram = new RAM(nextAddress, size);
	ram->base = nextAddress;
	ram->_deviceName = cDesc;
	if (nextAddress <= 0xFFFF) {
		m_memBlocks.push_back(ram);
	}
	nextAddress += size;
	return size;
}
RAM::RAM(Word offset, Word size)
{
	Device::Base(offset);
	Device::Size(size);
	_deviceName = "RAM";
}

RAM::~RAM()
{
}

Byte RAM::read(Word offset) {
	if (offset - base < size)
		return memory[(Word)(offset - base)];
	return 0xff;
}

void RAM::write(Word offset, Byte data) {
	if (offset - base < size)
		memory[(Word)(offset - base)] = data;
}

DWord Memory::AssignROM(std::string cDesc, Word size, const char* file = nullptr) {
	ROM* rom = new ROM(nextAddress, size);
	rom->base = nextAddress;
	rom->_deviceName = cDesc;
	if (file)
	{
		rom->setFilename(file);
		rom->load_hex(file, nextAddress);
	}
	if (nextAddress <= 0xFFFF) {
		m_memBlocks.push_back(rom);
	}
	nextAddress += size;
	return size;
}
ROM::ROM(Word offset, Word size)
{
	_deviceName = "ROM";
	Device::Base(offset);
	Device::Size(size);
}

ROM::~ROM()
{
}


Byte ROM::read(Word offset) {
	if (offset - base < size)
		return memory[(Word)(offset - base)];
	return 0xff;
}

void ROM::write(Word offset, Byte data) {	// NO OP
	(Word)offset;
	(Byte)data;
}


static Byte fread_hex_byte(FILE* fp)
{
	char str[3];
	long l;

	str[0] = fgetc(fp);
	str[1] = fgetc(fp);
	str[2] = '\0';

	l = strtol(str, NULL, 16);
	return (Byte)(l & 0xff);
}
static Word fread_hex_word(FILE* fp)
{
	Word ret;

	ret = fread_hex_byte(fp);
	ret <<= 8;
	ret |= fread_hex_byte(fp);

	return ret;
}
void ROM::load_hex(const char* filename, Word base) {
	FILE* fp;
	int done = 0;
	setFilename(filename);

#pragma warning(suppress : 4996)
	fp = fopen(filename, "r");
	if (!fp) {
		perror("filename");
		std::string err = "ROM file \"";
		err += filename;
		err += "\" not found!";
		//ErrorLogger::Log(err.c_str());
		Bus::Err(err.c_str());
		return;		// exit(EXIT_FAILURE);
	}

	while (!done) {
		Byte n, t;
		Word addr;
		Byte b;

		(void)fgetc(fp);
		n = fread_hex_byte(fp);
		addr = fread_hex_word(fp);
		t = fread_hex_byte(fp);
		if (t == 0x00) {
			while (n--) {
				b = fread_hex_byte(fp);
				if ((addr >= base) && (addr < ((DWord)base + size))) {
					memory[(Word)(addr - base)] = b;
					//bus->debug_write(addr - base, b);
				}
				else
				{
					if (bus == nullptr)
						bus = Bus::getInstance();
					bus->write(addr, b);
				}
				++addr;
			}
		}
		else if (t == 0x01) {
			done = 1;
		}
		// Read and discard checksum byte
		(void)fread_hex_byte(fp);
		if (fgetc(fp) == '\r') (void)fgetc(fp);
	}
}



DWord Memory::AssignREG(std::string cDesc, Word size, Byte(*cb)(REG* module, Word ofs, Byte data, bool bWasRead)) {
	REG* reg = new REG(nextAddress, size, cb);
	reg->_deviceName = cDesc;
	nextAddress += size;
	if (nextAddress <= 0xFFFF) {
		reg->callback = cb;
		m_memBlocks.push_back(reg);
	}
	reg->base = nextAddress;
	return size;
}
REG::REG(Word offset, Word size, Byte(*cb_callback)(REG*, Word, Byte, bool))
{
	callback = cb_callback;
	Device::Base(offset);
	Device::Size(size);
}

REG::~REG()
{

}







Byte Memory::read(Word ofs)
{
	for (int t = 0; t < m_memBlocks.size(); t++)
	{
		Word base = m_memBlocks[t]->Base();
		Word size = m_memBlocks[t]->Size();
		Word flr = ofs - base;

		if (flr < size) {
			Byte data = m_memBlocks[t]->read(ofs);
			RAM* ram = dynamic_cast<RAM*>(m_memBlocks[t]);
			if (ram != nullptr) {
				return ram->read(ofs);
			}
			ROM* rom = dynamic_cast<ROM*>(m_memBlocks[t]);
			if (rom != nullptr) {
				return rom->read(ofs);
			}
			REG* reg = dynamic_cast<REG*>(m_memBlocks[t]);
			if (reg != nullptr) {
				if (reg->callback)
					return reg->callback(reg, ofs, data, true);
			}
			return data;
		}
	}
	return 0xCC;
}
void Memory::write(Word offset, Byte data)
{
}
Word Memory::read_word(Word offset)
{
	return (Word)0;
}
void Memory::write_word(Word offset, Word data)
{
}
Byte Memory::debug_read(Word offset)
{
	return (Byte)0;
}
void Memory::debug_write(Word offset, Byte data)
{
}
Word Memory::debug_read_word(Word offset)
{
	return (Word)0;
}
void Memory::debug_write_word(Word offset, Word data)
{
}



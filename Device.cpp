/***********************************************
 * Device.cpp
 *
 * Base Device class
 ******************************/

#include "Bus.h"

#include "Device.h"

Device::Device(std::string name)
{
	_deviceName = name;
	//bus = Bus::getInstance();
}
Device::Device(Word offset, Word size) : base(offset), size(size), memory(size) {
	_deviceName = "Device";
	//bus = Bus::getInstance();
}

Device::~Device() 
{
}


Byte Device::read(Word offset) {
	if (offset - base < size)
		return memory[(Word)(offset - base)];
	return 0xCC;
}

void Device::write(Word offset, Byte data) {
	if (offset - base < size)
		memory[(Word)(offset - base)] = data;
}

Word Device::read_word(Word offset) {
	Word ret = (read(offset) << 8) | read(offset + 1);
	return ret;
}

void Device::write_word(Word offset, Word data) {
	Byte msb = (data >> 8) & 0xFF;
	Byte lsb = data & 0xff;
	write(offset, msb);
	write(offset + 1, lsb);
}
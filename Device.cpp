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

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

Device::~Device() 
{
}
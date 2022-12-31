/***********************************************
 * Device.cpp
 *
 * Base Device class
 ******************************/
#include "Device.h"

Device::Device(std::string name)
{
	_deviceName = name;
	//bus = Bus::getInstance();
}

//Word Device::MapDevice(MemoryMap* memmap, Word offset)
//{
//    // Defined only to serve as a template for inherited device objects.
//    // (this will never be called due to being an abstract base type.)
//    memmap->push({ offset, "", "" }); offset += 0;
//    memmap->push({ offset, "", "Base Device Registers (not-cool):" }); offset += 0;
//    memmap->push({ offset, "BASE_HDW_REG",	"Base Device Hardware Register" }); offset += 2;
//
//    return offset;
//}

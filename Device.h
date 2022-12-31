/***********************************************
 * Device.h
 * 
 * Base Device class
 ******************************/
#pragma once
#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "types.h"
#include "MemoryMap.h"
#include <string>

class Bus;

class Device
{
    friend class Bus;

public:
	Device(std::string name);
	virtual ~Device();

	// map this devices hardware registers (TODO: move to class Memory)
	virtual Word MapDevice(MemoryMap* memmap, Word offset) { return offset; }

	// abstract members
	virtual void OnInitialize() = 0;				// runs once after all devices are created
	virtual void OnEvent(SDL_Event* evnt) = 0;		// fires per SDL_Event
	virtual void OnCreate() = 0;					// fires when the object is created/recreated
	virtual void OnDestroy() = 0;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) = 0;	// fires each frame, for updates
	virtual void OnRender() = 0;					// render the current frames texture
    virtual void OnQuit() = 0;						// fires on exit -- reverses OnInitialize()

	// virtual accessors
	virtual Byte read(Word offset) = 0;
	virtual void write(Word offset, Byte data) = 0;
	virtual Word read_word(Word offset) = 0;
	virtual void write_word(Word offset, Word data) = 0;
	virtual Byte debug_read(Word offset) = 0;
	virtual void debug_write(Word offset, Byte data) = 0;
	virtual Word debug_read_word(Word offset) = 0;
	virtual void debug_write_word(Word offset, Word data) = 0;

	// virtual getters/setters
	virtual const char* Name() { return _deviceName.c_str(); }
	virtual void Name(std::string& text) { _deviceName = text; }
	virtual Word Base() { return base; }
	virtual void Base(Word ofs) { base = ofs; }
	virtual Word Size() { return size; }
	virtual void Size(Word pSize) { size = pSize; }

protected:
    std::string _deviceName = "??DEV??";
    Bus* bus = nullptr;

	Word base = 0;
	Word size = 0;
	std::vector<Byte> memory;	// memory mapped to this device
};

#endif // __DEVICE_H__
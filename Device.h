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
	virtual ~Device() {}

	virtual Word MapDevice(MemoryMap* memmap, Word offset) = 0;		// map this devices hardware registers

	virtual void OnInitialize() = 0;				// runs once after all devices are created
	virtual void OnEvent(SDL_Event* evnt) = 0;		// fires per SDL_Event
	virtual void OnCreate() = 0;					// fires when the object is created/recreated
	virtual void OnDestroy() = 0;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) = 0;	// fires each frame, for updates
	virtual void OnRender() = 0;					// render the current frames texture
    virtual void OnQuit() = 0;						// fires on exit -- reverses OnInitialize()

protected:
    std::string _deviceName = "??DEV??";
    Bus* bus = nullptr;
};

#endif // __DEVICE_H__
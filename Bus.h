/*********************************************************
 *  Bus.h (SINGLETON)
 *
 * Codename: TONI
 *
 *	"TONI" PICO #1 -- CORE ONE:	(6809 CPU Emulation / Debugger)
 *		- 6809 CPU emulation
 *			- Integrated Debugger
 *		- Main 64KB Memory Map
 *			- Receives Memory Event Dispatches from the other PICO as paged memory banks
 *			- Updates Hardware Registers via UART, SPI, or I2C @ 115200 baud 
 *				as master with "KIMI" PICO #2 acting as slave.
 *
 *	use: getInstance() to retrieve a static bus device pointer.
 *		call release() to destroy the Bus device.
 *
 *	This is the foundational container for every
 *	device in work in the system. It serves as a
 *  central event dispatcher to and from all of the
 *  attached devices. Every device attached to the
 *	Bus shall inherit from the Device class.
 *
 *	Also included here is support for error logging
 *	and system shut down events.
 * 
 * Copyright (C) 2023 by Jay Faries 
 ************************************/
#pragma once
#ifndef __BUS_H__
#define __BUS_H__

#include "types.h"
#include <vector>
#include "GFX.h"

class Device;
class GFX;
class Memory;
class FileIO;
class Keyboard;
class Gamepad;
class C6809;
class Math;

class Bus
{
private:
    Bus();      // private constructor
    ~Bus();     // private destructor

    static Bus* s_instance;
    static bool s_bIsRunning;

    static void _OnInitialize();           // runs once after all devices are created
    static void _OnEvent(SDL_Event* evnt); // fires per SDL_Event
    static void _OnCreate();               // fires when the object is created/recreated
    static void _OnDestroy();              // fires when the object is destroyed/lost focus
    static void _OnUpdate();               // fires each frame, for updates
    //static void _OnRender();               // render the current frames texture
    static void _OnQuit();                 // fires on exit -- reverses OnInitialize()

    std::vector<Device*> _devices;
    int m_fps = 0;    

public:
    Byte read(Word offset);
    void write(Word offset, Byte data);
    Word read_word(Word offset);
    void write_word(Word offset, Word data);

    Byte debug_read(Word offset);
    void debug_write(Word offset, Byte data);
    Word debug_read_word(Word offset);
    void debug_write_word(Word offset, Word data);

    Memory* getMemoryPtr() { return m_memory; }

    bool IsRunning() { return s_bIsRunning; }
    void IsRunning(bool isrunning) { s_bIsRunning = isrunning; }

    void AttachDevice(REG* reg) { _devices.push_back(reg); }

    static int getFPS();
    void run(); // main game loop
    static void CpuThread();
    bool bCpuEnabled = false;
    // bool bDebugMemDumping = false;

public:
    static std::string hex(Uint32 n, Uint8 d);

    // Error system
    static bool Err(const char* msg);

    // delete the copy constructor
    Bus(const Bus& obj) = delete;
    // use to fetch / create the only instance of Bus
    static Bus* getInstance();

    Memory* m_memory = nullptr;
    GFX *m_gfx = nullptr;
    C6809* m_cpu = nullptr;
    GfxDebug* gfxdebug = nullptr;    
    FileIO* m_file = nullptr;
    Keyboard* m_keyboard = nullptr;
    Gamepad* m_gamepad = nullptr;
    Math* m_math = nullptr;
};

#endif // __BUS_H__
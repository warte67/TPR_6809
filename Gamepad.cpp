/**** Gamepad.cpp ***************************************
 *
 * The Gamepad / Joystick device interface.
 *
 * Copyright (C) 2023 by Jay Faries
 ************************************/

#include <map>
#include "types.h"
#include "GFX.h"
#include "Bus.h"
#include "Memory.h"
#include "Gamepad.h"




 // memory-map callback function
Byte Gamepad::OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead)
{
	Bus* bus = Bus::getInstance();
	Gamepad* ptrGpad = dynamic_cast<Gamepad*>(reg);
	if (ptrGpad)
	{
		if (bWasRead)
		{
			data = ptrGpad->read(ofs);
		}
		else
		{
			// read only for all but the two dead band registers
			if (ofs == JOYS_1_DBND || ofs == JOYS_2_DBND)
				ptrGpad->write(ofs, data);
		}
	}
	return data;
}



Word Gamepad::MapDevice(MemoryMap* memmap, Word offset)
{
	int start_offset = offset;

	//memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Gamepad Hardware Registers:                   " }); offset += 0;
	memmap->push({ offset, "GPD_BEGIN", "start of keyboard hardware registers  " }); offset += 0;

	memmap->push({ offset, "JOYS_1_BTN",  "(Word) button bits: room for up to 16 buttons  (realtime)" }); offset += 2;
	memmap->push({ offset, "JOYS_1_DBND", "(Byte) PAD 1 analog deadband; default is 5   (read/write)" }); offset += 1;
	memmap->push({ offset, "JOYS_1_LTX",  "(char) PAD 1 LThumb-X position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_1_LTY",  "(char) PAD 1 LThumb-Y position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_1_RTX",  "(char) PAD 1 RThumb-X position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_1_RTY",  "(char) PAD 1 RThumb-Y position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_1_Z1",   "(char) PAD 1 left trigger        (0 - 127)     (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_1_Z2",   "(char) PAD 1 right trigger       (0 - 127)     (realtime)" }); offset += 1;
	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "JOYS_2_BTN",  "(Word) button bits: room for up to 16 buttons  (realtime)" }); offset += 2;
	memmap->push({ offset, "JOYS_2_DBND", "(Byte) PAD 2 analog deadband; default is 5   (read/write)" }); offset += 1;
	memmap->push({ offset, "JOYS_2_LTX",  "(char) PAD 2 LThumb-X position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_2_LTY",  "(char) PAD 2 LThumb-Y position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_2_RTX",  "(char) PAD 2 RThumb-X position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_2_RTY",  "(char) PAD 2 RThumb-Y position (-128 _ +127)   (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_2_Z1",   "(char) PAD 2 left trigger        (0 - 127)     (realtime)" }); offset += 1;
	memmap->push({ offset, "JOYS_2_Z2",   "(char) PAD 2 right trigger       (0 - 127)     (realtime)" }); offset += 1;

	memmap->push({ offset, "GPD_END", "end of keyboard hardware registers     " }); offset += 1;
	memmap->push({ offset, "", "" });

	return offset - start_offset;
}

Gamepad* Gamepad::Assign_Gamepad(MemoryMap* memmap, DWord& offset)
{
	Bus* bus = Bus::getInstance();
	Gamepad* ret = nullptr;
	// attach a Gamepad device:
	int fSize = Gamepad::MapDevice(memmap, offset);
	ret = new Gamepad(offset, fSize);
	ret->bus = bus;
	ret->memory = bus->m_memory;
	bus->AttachDevice(ret);		// bus->_devices.push_back(ret);
	bus->m_memory->ReassignReg(offset, ret, "GAMEPAD_HWD", fSize, Gamepad::OnCallback);
	ret->Base(offset);
	ret->Size(fSize);
	bus->m_memory->NextAddress(offset + fSize);
	offset += fSize;
	return ret;
}

Gamepad::Gamepad() : REG(0, 0)
{
	Device::_deviceName = "???Gamepad???";
	// this constructore is removed early.
	// dont use it for initialization
}

Gamepad::Gamepad(Word offset, Word size) : REG(offset, size)
{
	Device::_deviceName = "GAMEPAD";
	bus = Bus::getInstance();
	bus->m_gamepad = this;
	this->memory = bus->getMemoryPtr();
}

Gamepad::~Gamepad()
{
}


void Gamepad::OnInitialize() 
{
	InitButtonStates();
	OnCreate();
}
void Gamepad::OnQuit()
{
	OnDestroy();
}

void Gamepad::OnEvent(SDL_Event* evnt) 
{
	switch (evnt->type)
	{
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEREMAPPED:
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
		CloseControllers();
		OpenControllers();
		break;
	}
}
void Gamepad::OnCreate() 
{
	if (!bGamepadWasInit)
	{
		int ret = SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
		if (ret < 0)
		{
			std::string msg = "SDL could not initialize gamepads and/or joysticks! SDL_Error: ";
			msg += SDL_GetError();
			Bus::Err(msg.c_str());
			return;
		}
		// set the default analog dead band (dead zone)
		bus->debug_write(JOYS_1_DBND, 5);
		bus->debug_write(JOYS_2_DBND, 5);
		OpenControllers();
		bGamepadWasInit = true;
	}
}
void Gamepad::OnDestroy() 
{
	if (bGamepadWasInit)
	{
		CloseControllers();
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
		bGamepadWasInit = false;
	}
}
void Gamepad::OnUpdate(float fElapsedTime) 
{
	// update button registers
	if (state[0].bIsActive)
		bus->debug_write_word(JOYS_1_BTN, EncodeButtonRegister(0));
	else
		bus->debug_write_word(JOYS_1_BTN, 0xffff);
	if (state[1].bIsActive)
		bus->debug_write_word(JOYS_2_BTN, EncodeButtonRegister(1));
	else
		bus->debug_write_word(JOYS_2_BTN, 0xffff);
	EncodeAxesRegister(0);
	EncodeAxesRegister(1);
}

// ******************************************************************

void Gamepad::InitButtonStates()
{
	Byte btn = 0;
	gpadBtnMap.clear();
	joysBtnMap.clear();

	gpadBtnMap[SDL_CONTROLLER_BUTTON_A]				= btn++;	// GPAD_BTN::BTN_A
	gpadBtnMap[SDL_CONTROLLER_BUTTON_B]				= btn++;	// GPAD_BTN::BTN_B
	gpadBtnMap[SDL_CONTROLLER_BUTTON_X]				= btn++;	// GPAD_BTN::BTN_X
	gpadBtnMap[SDL_CONTROLLER_BUTTON_Y]				= btn++;	// GPAD_BTN::BTN_Y
	gpadBtnMap[SDL_CONTROLLER_BUTTON_BACK]			= btn++;	// GPAD_BTN::BTN_BACK
	gpadBtnMap[SDL_CONTROLLER_BUTTON_START]			= btn++;	// GPAD_BTN::BTN_START
	gpadBtnMap[SDL_CONTROLLER_BUTTON_LEFTSHOULDER]	= btn++;	// GPAD_BTN::BTN_LS
	gpadBtnMap[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = btn++;	// GPAD_BTN::BTN_RS
	gpadBtnMap[SDL_CONTROLLER_BUTTON_DPAD_UP]		= btn++;	// GPAD_BTN::BTN_DPAD_UP
	gpadBtnMap[SDL_CONTROLLER_BUTTON_DPAD_DOWN]		= btn++;	// GPAD_BTN::BTN_DPAD_DOWN
	gpadBtnMap[SDL_CONTROLLER_BUTTON_DPAD_LEFT]		= btn++;	// GPAD_BTN::BTN_DPAD_LEFT
	gpadBtnMap[SDL_CONTROLLER_BUTTON_DPAD_RIGHT]	= btn++;	// GPAD_BTN::BTN_DPAD_RIGHT
	gpadBtnMap[SDL_CONTROLLER_BUTTON_GUIDE]			= btn++;	// GPAD_BTN::BTN_GUIDE

	btn = 0;
	joysBtnMap[JOYS_BTN::BTN_1]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_2]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_3]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_4]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_5]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_6]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_7]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_8]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_HAT_UP]	= btn++;
	joysBtnMap[JOYS_BTN::BTN_HAT_DOWN]	= btn++;
	joysBtnMap[JOYS_BTN::BTN_HAT_LEFT]	= btn++;
	joysBtnMap[JOYS_BTN::BTN_HAT_RIGHT]	= btn++;
	joysBtnMap[JOYS_BTN::BTN_9]			= btn++;
	joysBtnMap[JOYS_BTN::BTN_10]		= btn++;
	joysBtnMap[JOYS_BTN::BTN_11]		= btn++;
	joysBtnMap[JOYS_BTN::BTN_12]		= btn++;
}


void Gamepad::OpenControllers()
{
	int num_joysticks = SDL_NumJoysticks();
	for (int index = 0; index < num_joysticks; index++)
	{
		if (SDL_IsGameController(index)) // is a gamepad
		{
			state[index].bIsGamepad = true;
			state[index].name = SDL_GameControllerNameForIndex(index);
			state[index].controller = SDL_GameControllerOpen(index);
			if (state[index].controller == nullptr)
			{
				std::string msg = "SDL could not open a gamepad! SDL_Error: ";
				msg += SDL_GetError();
				Bus::Err(msg.c_str());
				return;
			}
			state[index].bIsActive = true;
		}
		else // is a joystick
		{
			state[index].bIsGamepad = false;
			state[index].joystick = SDL_JoystickOpen(index);
			if (state[index].joystick == nullptr)
			{
				std::string msg = "SDL could not open a joystick! SDL_Error: ";
				msg += SDL_GetError();
				Bus::Err(msg.c_str());
				return;
			}
			state[index].name = SDL_JoystickName(state[index].joystick);
			state[index].bIsActive = true;
		}
	}
}

void Gamepad::CloseControllers()
{
	state[0].bIsActive = false;
	bus->debug_write_word(JOYS_1_BTN, 0xffff);
	state[1].bIsActive = false;
	bus->debug_write_word(JOYS_2_BTN, 0xffff);
}


Word Gamepad::EncodeButtonRegister(int id)
{
	id %= 2;
	Word ret = 0xffff;
	if (state[id].bIsActive)
	{
		ret = 0;
		// encode only the first 16 buttons
		for (int btn = 0; btn < 16; btn++)
		{
			// gamepad?
			if (state[id].bIsGamepad)
			{
				if (SDL_GameControllerGetButton(state[id].controller, (SDL_GameControllerButton)btn))
					ret |= (1 << gpadBtnMap[btn]);
			}
			else // joystick
			{
				if (SDL_JoystickGetButton(state[id].joystick, btn))
					ret |= (1 << joysBtnMap[btn]);
				// encode hat
				Byte hat = SDL_JoystickGetHat(state[id].joystick, 0);
				if (hat == SDL_HAT_UP || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_LEFTUP)
					ret |= 0x0100;
				if (hat == SDL_HAT_DOWN || hat == SDL_HAT_RIGHTDOWN || hat == SDL_HAT_LEFTDOWN)
					ret |= 0x0200;
				if (hat == SDL_HAT_LEFT || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_LEFTDOWN)
					ret |= 0x0400;
				if (hat == SDL_HAT_RIGHT || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_RIGHTDOWN)
					ret |= 0x0800;
			}
		}
	}
	return ret;
}


void Gamepad::EncodeAxesRegister(int id)
{
	id %= 2;
	if (state[id].bIsActive)
	{
		Sint8 deadband = read(JOYS_1_DBND);
		if (id == 1)
			Sint8 deadband = read(JOYS_2_DBND);

		if (state[id].bIsGamepad)	// is gamepad
		{
			Sint8 LTX = (SDL_GameControllerGetAxis(state[id].controller, SDL_CONTROLLER_AXIS_LEFTX) & 0xff00) >> 8;
			if (LTX < 0 && LTX > -deadband)	LTX = 0;
			if (LTX > 0 && LTX < deadband)	LTX = 0;
			Sint8 LTY = (SDL_GameControllerGetAxis(state[id].controller, SDL_CONTROLLER_AXIS_LEFTY) & 0xff00) >> 8;
			if (LTY < 0 && LTY > -deadband)	LTY = 0;
			if (LTY > 0 && LTY < deadband)	LTY = 0;
			Sint8 RTX = (SDL_GameControllerGetAxis(state[id].controller, SDL_CONTROLLER_AXIS_RIGHTX) & 0xff00) >> 8;
			if (RTX < 0 && RTX > -deadband)	RTX = 0;
			if (RTX > 0 && RTX < deadband)	RTX = 0;
			Sint8 RTY = (SDL_GameControllerGetAxis(state[id].controller, SDL_CONTROLLER_AXIS_RIGHTY) & 0xff00) >> 8;
			if (RTY < 0 && RTY > -deadband)	RTY = 0;
			if (RTY > 0 && RTY < deadband)	RTY = 0;
			Sint8 Z1 = SDL_GameControllerGetAxis(state[id].controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >> 8;
			Sint8 Z2 = SDL_GameControllerGetAxis(state[id].controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) >> 8;

			if (id == 0)
			{
				bus->debug_write(JOYS_1_LTX, LTX);
				bus->debug_write(JOYS_1_LTY, LTY);
				bus->debug_write(JOYS_1_RTX, RTX);
				bus->debug_write(JOYS_1_RTY, RTY);
				bus->debug_write(JOYS_1_Z1, Z1);
				bus->debug_write(JOYS_1_Z2, Z2);
			}
			else
			{
				bus->debug_write(JOYS_2_LTX, LTX);
				bus->debug_write(JOYS_2_LTY, LTY);
				bus->debug_write(JOYS_2_RTX, RTX);
				bus->debug_write(JOYS_2_RTY, RTY);
				bus->debug_write(JOYS_2_Z1, Z1);
				bus->debug_write(JOYS_2_Z2, Z2);
			}
		}
		else  // is joystick
		{
			Sint8 LTX = (SDL_JoystickGetAxis(state[id].joystick, 0) & 0xff00) >> 8;
			if (LTX < 0 && LTX > -deadband)	LTX = 0;
			if (LTX > 0 && LTX < deadband)	LTX = 0;
			Sint8 LTY = (SDL_JoystickGetAxis(state[id].joystick, 1) & 0xff00) >> 8;
			if (LTY < 0 && LTY > -deadband)	LTY = 0;
			if (LTY > 0 && LTY < deadband)	LTY = 0;
			// RTX maps the hat
			Byte hat = SDL_JoystickGetHat(state[id].joystick, 0);
			Sint8 RTX = 0;
			Sint8 RTY = 0;
			if (hat == SDL_HAT_UP || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_LEFTUP)
				RTY = -128;
			if (hat == SDL_HAT_DOWN || hat == SDL_HAT_RIGHTDOWN || hat == SDL_HAT_LEFTDOWN)
				RTY = 127;
			if (hat == SDL_HAT_LEFT || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_LEFTDOWN)
				RTX = -128;
			if (hat == SDL_HAT_RIGHT || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_RIGHTDOWN)
				RTX = 127;
			// throttle lever
			Uint8 Z1 = SDL_JoystickGetAxis(state[id].joystick, 3) >> 8;
			Z1 = 255 - (Z1 + 128);

			// rudder axis
			Sint8 Z2 = SDL_JoystickGetAxis(state[id].joystick, 2) >> 8;
			if (Z2 < 0 && Z2 > -deadband * 3)	Z2 = 0;
			if (Z2 > 0 && Z2 < deadband * 3)	Z2 = 0;

			if (id == 0)
			{
				bus->debug_write(JOYS_1_LTX, LTX);
				bus->debug_write(JOYS_1_LTY, LTY);
				bus->debug_write(JOYS_1_RTX, RTX);
				bus->debug_write(JOYS_1_RTY, RTY);
				bus->debug_write(JOYS_1_Z1, Z1);
				bus->debug_write(JOYS_1_Z2, Z2);
			}
			else
			{
				bus->debug_write(JOYS_2_LTX, LTX);
				bus->debug_write(JOYS_2_LTY, LTY);
				bus->debug_write(JOYS_2_RTX, RTX);
				bus->debug_write(JOYS_2_RTY, RTY);
				bus->debug_write(JOYS_2_Z1, Z1);
				bus->debug_write(JOYS_2_Z2, Z2);
			}
		}
	}
}

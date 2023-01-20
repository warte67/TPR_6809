// * Keyboard.h ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico #2
// *		Dispatches Keyboard Events to PICO #1
// *		Emulates the Keyboard Hardware Registers
// ************************************
#pragma once
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <map>
#include <queue>
#include <array>
#include "XK_KEYCODES.h"

class Bus;
class Memory;

class Keyboard : public REG
{
	friend class Bus;
	friend class Memory;

public:

	Keyboard();
	Keyboard(Word offset, Word size);
	virtual ~Keyboard();

	// static members
	static Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);
	static Keyboard* Assign_Keyboard(MemoryMap* memmap, DWord& offset);
	static Word MapDevice(MemoryMap* memmap, Word offset);

	// abstract members
	virtual void OnInitialize() override;				// runs once after all devices are created
	virtual void OnEvent(SDL_Event* evnt) override;		// fires per SDL_Event
	virtual void OnCreate() override;					// fires when the object is created/recreated
	virtual void OnDestroy() override;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) override;	// fires each frame, for updates
	//virtual void OnRender() override;					// render the current frames texture
	virtual void OnQuit() override;						// fires on exit -- reverses OnInitialize()

	XKey::XK TranslateSDLtoXKey(SDL_Scancode k);
	XKey::XK AscToXKey(Byte asc);
	char XKeyToAscii(XKey::XK xKey);
	bool IsKeyDown(Byte xk);
	Byte charScanQueue();
	Byte charPopQueue();
	int charQueueLen();
	void Clear();

private:
	Bus* bus = nullptr;
	Memory* memory = nullptr;

	std::map<XKey::XK, int> keyMap;	
	std::vector<std::tuple<XKey::XK, char, char>> xkToAsciiMap;
	std::queue <Byte> charQueue;		// ascii character queue
	std::array <Byte, 256> editBuffer;	// internal line edit character buffer
	Byte edt_bfr_csr = 0;				// internal edit character buffer cursor position
};

#endif // __KEYBOARD_H__


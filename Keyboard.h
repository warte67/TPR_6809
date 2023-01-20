// * Keyboard.h ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico #2
// *		Dispatches Keyboard Events to PICO #1
// *		Emulates the Keyboard Hardware Registers
// ************************************
#pragma once
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

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

private:
	Bus* bus = nullptr;
	Memory* memory = nullptr;

};

#endif // __KEYBOARD_H__


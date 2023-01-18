// * FileIO.h ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		Implementing File Input / Output
// *		with the onboad flash ram and
// *		the SDCARD
// ************************************
#pragma once
#ifndef __FILEIO_H__
#define __FILEIO_H__

class Bus;
class Memory;

class FileIO : public REG
{
	friend class Bus;
	friend class Memory;

public:

	FileIO();
	FileIO(Word offset, Word size);
	virtual ~FileIO();

	static Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);

	// map this devices hardware registers (TODO: move to class Memory)
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

#endif // __FILEIO_H__
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

	// static members
	static Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);
	static FileIO* Assign_FileIO(MemoryMap* memmap, DWord& offset);
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


/*******


Basic File Operating System:
	- List Directory
	- Make Directory
	- Change Directories
	- Rename Directory
	- Remove Directory
	- Create file
	- Delete File
	- Rename file
	- Read File
	- Write File
	- Append File
	- Copy File
	- LoadBin			; load a binary file to a specified memory buffer (size)
	- LoadHex			; load an IntelHex format file (with/without EXEC vector)
	- EXEC <address>	; cpu.PC = address (or EXEC vector if non-zero)

Basic FILE I/O:

	- FP = (Sint32)file_pointer - internal to FileIO
	- FH = (Byte) file_handle
	- FH = fopen(string::filename, string::mode)	// mode = read, write, or append
	- data = fread(FH)
	- fwrite(FH, data)
	- fseek(FH, (signed)offset, whence) where whence = SEEK_START, SEEK_CUR, and SEEK_END
		; SEEK_START	= move the file_pointer to 0 + offset
		; SEEK_CUR		= move the file_pointer to file_pinter + offset
		; SEEK_END		= move the file_pointer to eof + offset (negative offset assumed)
	- fclose(FH)


*******/
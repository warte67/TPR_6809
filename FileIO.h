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

Hardware Registers:

	FIO_FLAGS: (AKA Error Flags)
		bit 7:	file not found
		bit 6:  end of file
		bit 5:	buffer overrun
		bit 4:  
		bit 3:	
		bit 2:	
		bit 1:	
		bit 0:	

	FIO_COMMAND		=	(Byte) OnWrite - command to execute
	FIO_INDEX		=	(Byte) file page index (Page ZERO = Base COMMAND)
	; paged arguments registers
		FIO_HANDLE		=	(Byte) file handle or ZERO if inactive
		FIO_FILE_BFR	=	(Char Array 80)  path/filename
		FIO_BUFFER		=	(Word) address of the current read/write buffer
		FIO_BFR_SIZE	=	(Word) size of the read/write buffer



Base File Operating System Commands:
	- List Directory
	- Make Directory
	- Change Directories (presumes CURRENT DIRECTORY pointer)
	- Rename Directory
	- Remove Directory
	- Delete File		
	- Rename file
	- Read File			; open for reading or error if not found
	- Write File		; append if pre-existing or create for write if not
	- Close File		; be sure to close files to avoid corruption
	- Copy File
	- LoadBin			; load a binary file to a specified memory buffer (size)
	- LoadHex			; load an IntelHex format file (with/without EXEC vector)
	- EXEC <address>	; cpu.PC = address (or EXEC vector if non-zero)

Basic FILE I/O:

	- FP = (Word)file_pointer - internal to FileIO (no bin files larger than 64k)
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
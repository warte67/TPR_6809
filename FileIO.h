/* FileIO.h ***************************************
 *
 *   Acts to emulate a Raspberry PI Pico
 *		Implementing File Input / Output
 *		with the onboad flash ram and
 *		the SDCARD
 * 
 * Copyright (C) 2023 by Jay Faries
 ************************************/
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

	void load_hex(const char* filename);

private:
	// file system commands
	void _cmd_reset();				// $00 = Reset/Null
	void _cmd_open_read();			// $01 = Open/Create Binary File for Reading
	void _cmd_open_write();			// $02 = Open/Create Binary File for Writing
	void _cmd_open_append();		// $03 = Open/Create Binary File for Append
	void _cmd_close();				// $04 = Close File
	void _cmd_read_byte();			// $05 = Read Byte
	void _cmd_write_byte();			// $06 = Write Byte
	void _cmd_load_hex();			// $07 = Load Hex Format File
	void _cmd_write_hex_line();		// $08 = Write Hex Format Line
	void _cmd_get_file_length();	// $09 = Get File Length (FIO_BFRLEN = file length
	void _cmd_load_binary();		// $0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)
	void _cmd_save_binary();		// $0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN)
	void _cmd_list_dir();			// $0C = (not yet designed) List Directory
	void _cmd_make_dir();			// $0D = Make Directory
	void _cmd_change_dir();			// $0E = Change Directory
	void _cmd_rename_dir();			// $0F = Rename Directory
	void _cmd_remove_dir();			// $10 = Remove Directory
	void _cmd_delete_file();		// $11 = Delete File
	void _cmd_rename_file();		// $12 = Rename file
	void _cmd_copy_file();			// $13 = Copy File
	void _cmd_seek_start();			// $14 = Seek Start
	void _cmd_seek_current();		// $15 = Seek Current
	void _cmd_seek_end();			// $16 = Seek End

private:
    Bus* bus = nullptr;
    Memory* memory = nullptr;


	Byte _err_flags = 0;
	Byte _command = 0;
	Byte _file_handle = 0;	// index into an array of FILE entries
	Word _buffer_offset = 0;
	Word _buffer_length = 0;
	Word _seek_offset;
	char _filepath[256] = "./";			// Hardware Register working path
	
	Byte _ret_index = 0;
	std::vector<std::string> _files;


};

#endif // __FILEIO_H__


/*******

Hardware Registers:


		
	FIO_ERR_FLAGS:
		bit 7:	file not found
		bit 6:  end of file
		bit 5:	buffer overrun
		bit 0-4: reserved
	FIO_COMMAND		(Byte) OnWrite - command to execute
		$00 = Reset/Null                          
		$01 = Open/Create Binary File for Reading 
		$02 = Open/Create Binary File for Writing 
		$03 = Open/Create Binary File for Append  
		$04 = Close File                          
		$05 = Read Byte                           
		$06 = Write Byte                          
		$07 = Load Hex Format File        
		$08 = Write Hex Format Line
		$09 = Get File Length (FIO_BFRLEN = byte length of the file)
		$0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)
		$0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN)
		$0C = (not yet designed) List Directory
		$0D = Make Directory
		$0E = Change Directory
		$0F = Rename Directory
		$10 = Remove Directory
		$11 = Delete File		
		$12 = Rename file
		$13 = Copy File
		$14 = Seek Start
		$15 = Seek Current
		$16 = Seek End


	FIO_HANDLE		(Byte) file handle or ZERO          
	FIO_BFROFS		(Word) start of I/O buffer
	FIO_BFRLEN		(Word) length of I/O buffer
	FIO_SEEKOFS		(Word) seek offset
	FIO_FILEPATH	(Char Array 256) fixed file path buffer   




 **** NOTES: ***********************************************************

Base File Operating System Commands:
	- List Directory
	- Make Directory
	- Change Directory
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
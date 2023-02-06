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
	// helper functions
	int _FindOpenFileSlot();

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
	// ...							// $17 = SYSTEM: Shutdown
	void _cmd_read_compilation();	// $18 = SYSTEM: Load Compilation Date

private:
    Bus* bus = nullptr;
    Memory* memory = nullptr;


	Byte _err_flags = 0;
	Byte _command = 0;
	Byte _file_handle = 0;					// index into an array of FILE entries
	Word _buffer_offset = 0;
	Word _buffer_length = 0;
	char _filepath[256] = "./";				// Hardware Register working path
	Byte _io_data = 0;						// read / write character

	const int _FILESTREAMMAX = 16;			// max number of file streams
	std::vector<FILE*> _vecFileStream;		// FILE*[16];
	
	Byte _ret_index = 0;
	std::vector<std::string> _files;


};

#endif // __FILEIO_H__


/*******

*******/
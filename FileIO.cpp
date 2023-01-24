// * FileIO.cpp ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		Implementing File Input / Output
// *		with the onboad flash ram and
// *		the SDCARD
// ************************************

#include <stdio.h>
//#include <string>
#include <iostream>
#include <sstream>
#include <filesystem>
//#include <format>


#include "types.h"
#include "Bus.h"
#include "Memory.h"
#include "C6809.h"
#include "FileIO.h"

//     FIO_BEGIN = 0x181e,        // start of file i/o hardware registers
//  FIO_ERR_FLAGS = 0x181e,       // (Byte) file i/o system flags:
//                                //      bit 7:  file not found
//                                //      bit 6:  end of file
//                                //      bit 5:  buffer overrun
//                                //      bit 4:  wrong file type
//                                //      bit 0-3: not yet assigned
//   FIO_COMMAND = 0x181f,        // (Byte) OnWrite - command to execute
//                                //      $00 = Reset/Null
//                                //      $01 = Open/Create Binary File for Reading
//                                //      $02 = Open/Create Binary File for Writing
//                                //      $03 = Open/Create Binary File for Append
//                                //      $04 = Close File
//                                //      $05 = Read Byte
//                                //      $06 = Write Byte
//                                //      $07 = Load Hex Format File
//                                //      $08 = Write Hex Format Line
//                                //      $09 = Get File Length (FIO_BFRLEN = file length
//                                //      $0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)
//                                //      $0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN)
//                                //      $0C = (not yet designed) List Directory
//                                //      $0D = Make Directory
//                                //      $0E = Change Directory
//                                //      $0F = Rename Directory
//                                //      $10 = Remove Directory
//                                //      $11 = Delete File
//                                //      $12 = Rename file
//                                //      $13 = Copy File
//                                //      $14 = Seek Start
//                                //      $15 = Seek Current
//                                //      $16 = Seek End
//    FIO_HANDLE = 0x1820,        // (Byte) file handle or ZERO
//    FIO_BFROFS = 0x1821,        // (Word) start of I/O buffer
//    FIO_BFRLEN = 0x1822,        // (Word) length of I/O buffer
//   FIO_SEEKOFS = 0x1823,        // (Word) seek offset
//  FIO_RET_COUNT = 0x1827,       // (Byte) number of return entries
//  FIO_RET_INDEX = 0x1828,       // (Byte) command return index
//  FIO_RET_BUFFER = 0x1829,      // (Char Array 256) paged return buffer
//  FIO_FILEPATH = 0x1824,        // (Char Array 256) file path and argument buffer
//       FIO_END = 0x1924,        // end of file i/o hardware registers


// memory-map callback function
Byte FileIO::OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead)
{
	Bus* bus = Bus::getInstance();
	FileIO* ptrFile = dynamic_cast<FileIO*>(reg);
	if (ptrFile)
	{
		if (ofs >= FIO_BEGIN && ofs <= FIO_END)
		{
			if (bWasRead)
			{	// READ FROM
				if (ofs == FIO_ERR_FLAGS)		data = ptrFile->_err_flags;
				else if (ofs == FIO_COMMAND)	data = ptrFile->_command;
				else if (ofs == FIO_HANDLE)		data = ptrFile->_file_handle;
				else if (ofs == FIO_BFROFS)		data = (ptrFile->_buffer_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFROFS + 1)	data = (ptrFile->_buffer_offset & 0xFF00) | (data << 0);
				else if (ofs == FIO_BFRLEN)		data = (ptrFile->_buffer_length & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFRLEN + 1)	data = (ptrFile->_buffer_length & 0xFF00) | (data << 0);
				else if (ofs == FIO_SEEKOFS)		data = (ptrFile->_seek_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_SEEKOFS + 1)	data = (ptrFile->_seek_offset & 0xFF00) | (data << 0);

				else if (ofs == FIO_RET_COUNT)	data = ptrFile->_files.size();
				else if (ofs == FIO_RET_INDEX)	data = ptrFile->_ret_index;

				else if (ofs >= FIO_RET_BUFFER && ofs < FIO_RET_BUFFER + 256)
				{
					Byte i = (ofs - FIO_RET_BUFFER) % 255;
					data = 0;
					if (ptrFile->_files.size()>0)
					{
						if (i >= ptrFile->_files[ptrFile->_ret_index].size())
							data = 0;
						else
							data = ptrFile->_files[ptrFile->_ret_index].at(i);
						//ptrFile->_files[ptrFile->_ret_index].at(256) = 0;	// ensure a trailing null
					}
				}

				else if (ofs >= FIO_FILEPATH && ofs < FIO_FILEPATH+256)
				{
					Byte i = (ofs - FIO_FILEPATH) % 255;
					data = ptrFile->_filepath[i];
					//ptrFile->_filepath[255] = 0;	// enforce atleast one NULL terminator
				}

				bus->debug_write(ofs, data);	// update the memory for debug_reads
			}
			else
			{	// WRITE TO
				if (ofs == FIO_ERR_FLAGS)
				{
					ptrFile->_err_flags = data;
					bus->debug_write(FIO_ERR_FLAGS, data);
				}
				else if (ofs == FIO_COMMAND)
				{
					switch (data)
					{
					case 0x00:	ptrFile->_cmd_reset();					break;
					case 0x01:	ptrFile->_cmd_open_read();				break;
					case 0x02:	ptrFile->_cmd_open_write();				break;
					case 0x03:	ptrFile->_cmd_open_append();			break;
					case 0x04:	ptrFile->_cmd_close();					break;
					case 0x05:	ptrFile->_cmd_read_byte();				break;
					case 0x06:	ptrFile->_cmd_write_byte();				break;
					case 0x07:	ptrFile->_cmd_load_hex();				break;		// load_hex
					case 0x08:	ptrFile->_cmd_write_hex_line();			break;
					case 0x09:	ptrFile->_cmd_get_file_length();		break;
					case 0x0A:	ptrFile->_cmd_load_binary();			break;
					case 0x0B:	ptrFile->_cmd_save_binary();			break;
					case 0x0C:	ptrFile->_cmd_list_dir();				break;
					case 0x0D:	ptrFile->_cmd_make_dir();				break;
					case 0x0E:	ptrFile->_cmd_change_dir();				break;
					case 0x0F:	ptrFile->_cmd_rename_dir();				break;
					case 0x10:	ptrFile->_cmd_remove_dir();				break;
					case 0x11:	ptrFile->_cmd_delete_file();			break;
					case 0x12:	ptrFile->_cmd_rename_file();			break;
					case 0x13:	ptrFile->_cmd_copy_file();				break;
					case 0x14:	ptrFile->_cmd_seek_start();				break;
					case 0x15:	ptrFile->_cmd_seek_current();			break;
					case 0x16:	ptrFile->_cmd_seek_end();				break;
					case 0x17:	bus->IsRunning(false);					break;

					default:
						break;
					}
				}
				else if (ofs == FIO_BFROFS)
					ptrFile->_buffer_offset = (ptrFile->_buffer_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFROFS + 1)
					ptrFile->_buffer_offset = (ptrFile->_buffer_offset & 0xFF00) | (data << 0);

				else if (ofs == FIO_BFRLEN)
					ptrFile->_buffer_length = (ptrFile->_buffer_length & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFRLEN + 1)
					ptrFile->_buffer_length = (ptrFile->_buffer_length & 0xFF00) | (data << 0);

				else if (ofs == FIO_SEEKOFS)
					ptrFile->_seek_offset = (ptrFile->_seek_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_SEEKOFS + 1)
					ptrFile->_seek_offset = (ptrFile->_seek_offset & 0xFF00) | (data << 0);

				else if (ofs == FIO_RET_INDEX)
				{
					ptrFile->_ret_index = data;
					bus->debug_write(FIO_RET_INDEX, data);
				}


				else if (ofs >= FIO_FILEPATH && ofs <= FIO_FILEPATH+256)
				{
					Word i = (ofs - FIO_FILEPATH);
					if (i >= FIO_FILEPATH + 256)   i = FIO_FILEPATH + 255;
					ptrFile->_filepath[i] = data;
				}

				bus->debug_write(ofs, data);
			}
		}
	}
	return data; 
}

FileIO* FileIO::Assign_FileIO(MemoryMap* memmap, DWord& offset)
{
	Bus* bus = Bus::getInstance();
	FileIO* ret = nullptr;
	// attach a FileIO device:
	int fSize = FileIO::MapDevice(memmap, offset);
	ret = new FileIO(offset, fSize);
	ret->bus = bus;
	ret->memory = bus->m_memory;	
	bus->AttachDevice(ret);		// bus->_devices.push_back(ret);
	bus->m_memory->ReassignReg(offset, ret, "FILEIO_HDW", fSize, FileIO::OnCallback);

	ret->Base(offset);
	ret->Size(fSize);

	bus->m_memory->NextAddress(offset + fSize);

	offset += fSize;

	return ret;
}

Word FileIO::MapDevice(MemoryMap* memmap, Word offset) 
{ 
	int start_offset = offset;

	//memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "File I/O Hardware Registers:                   " }); offset += 0;
	memmap->push({ offset, "FIO_BEGIN", "start of file i/o hardware registers  " }); offset += 0;

	memmap->push({ offset, "FIO_ERR_FLAGS", "(Byte) file i/o system flags:     " }); offset += 1;
	memmap->push({ offset, "", ">    bit 7:	file not found                     " }); offset += 0;
	memmap->push({ offset, "", ">    bit 6:  end of file                       " }); offset += 0;
	memmap->push({ offset, "", ">    bit 5:	buffer overrun                     " }); offset += 0;
	memmap->push({ offset, "", ">    bit 4: wrong file type                    " }); offset += 0;
	memmap->push({ offset, "", ">    bit 3: directory not found                " }); offset += 0;
	memmap->push({ offset, "", ">    bit 0-2: not yet assigned                 " }); offset += 0;

	memmap->push({ offset, "FIO_COMMAND", "(Byte) OnWrite - command to execute " }); offset += 1;
	memmap->push({ offset, "", ">    $00 = Reset/Null                          " }); offset += 0;
	memmap->push({ offset, "", ">    $01 = Open/Create Binary File for Reading " }); offset += 0;
	memmap->push({ offset, "", ">    $02 = Open/Create Binary File for Writing " }); offset += 0;
	memmap->push({ offset, "", ">    $03 = Open/Create Binary File for Append  " }); offset += 0;
	memmap->push({ offset, "", ">    $04 = Close File                          " }); offset += 0;
	memmap->push({ offset, "", ">    $05 = Read Byte                           " }); offset += 0;
	memmap->push({ offset, "", ">    $06 = Write Byte                          " }); offset += 0;
	memmap->push({ offset, "", ">    $07 = Load Hex Format File                " }); offset += 0;
	memmap->push({ offset, "", ">    $08 = Write Hex Format Line               " }); offset += 0;
	memmap->push({ offset, "", ">    $09 = Get File Length (FIO_BFRLEN = file length  " }); offset += 0;
	memmap->push({ offset, "", ">    $0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)	 " }); offset += 0;
	memmap->push({ offset, "", ">    $0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN) " }); offset += 0;
	memmap->push({ offset, "", ">    $0C = (not yet designed) List Directory   " }); offset += 0;
	memmap->push({ offset, "", ">    $0D = Make Directory                      " }); offset += 0;
	memmap->push({ offset, "", ">    $0E = Change Directory                    " }); offset += 0;
	memmap->push({ offset, "", ">    $0F = Rename Directory                    " }); offset += 0;
	memmap->push({ offset, "", ">    $10 = Remove Directory                    " }); offset += 0;
	memmap->push({ offset, "", ">    $11 = Delete File                         " }); offset += 0;
	memmap->push({ offset, "", ">    $12 = Rename file                         " }); offset += 0;
	memmap->push({ offset, "", ">    $13 = Copy File                           " }); offset += 0;
	memmap->push({ offset, "", ">    $14 = Seek Start                          " }); offset += 0;
	memmap->push({ offset, "", ">    $15 = Seek Current                        " }); offset += 0;
	memmap->push({ offset, "", ">    $16 = Seek End                            " }); offset += 0;
	memmap->push({ offset, "", ">    $17 = SYSTEM: Shutdown                    " }); offset += 0;

	memmap->push({ offset, "FIO_HANDLE",    "(Byte) file handle or ZERO          " }); offset += 1;
	memmap->push({ offset, "FIO_BFROFS",    "(Word) start of I/O buffer          " }); offset += 1;
	memmap->push({ offset, "FIO_BFRLEN",    "(Word) length of I/O buffer         " }); offset += 2;
	memmap->push({ offset, "FIO_SEEKOFS",   "(Word) seek offset                  " }); offset += 2;
										    
	memmap->push({ offset, "FIO_RET_COUNT", "(Byte) number of return entries     " }); offset += 1;
	memmap->push({ offset, "FIO_RET_INDEX", "(Byte) command return index         " }); offset += 1;
	memmap->push({ offset, "FIO_RET_BUFFER","(Char Array 256) paged return buffer" }); offset += 256;

	memmap->push({ offset, "FIO_FILEPATH", "(Char Array 256) file path and argument buffer    " }); offset += 256;
	memmap->push({ offset, "FIO_END", "end of file i/o hardware registers     " }); offset += 1;
	memmap->push({ offset, "", "" });

	return offset - start_offset; 
}


//// 


FileIO::FileIO() : REG(0, 0)
{
	Device::_deviceName = "???FileIO???";
	// this constructore is removed early.
	// dont use it for initialization
}

FileIO::FileIO(Word offset, Word size) : REG(offset, size) 
{
	Device::_deviceName = "FILEIO";
	bus = Bus::getInstance();
	bus->m_file = this;

	this->memory = bus->getMemoryPtr();
}

FileIO::~FileIO() 
{
}


void FileIO::OnInitialize() 
{
	//printf("FileIO::OnInitialize()\n");

	// null out the _filepath[] array
	for (auto& a : _filepath)
		a = 0;
}

void FileIO::OnEvent(SDL_Event* evnt) {}
void FileIO::OnCreate() {}
void FileIO::OnDestroy() {}
void FileIO::OnUpdate(float fElapsedTime) 
{
	//	printf("CPU_CLK_DIV: $%02X\n", bus->read(CPU_CLK_DIV));
}
void FileIO::OnQuit() {}


////  Intel Hex Load //////////////////////////////
static Byte fio_fread_hex_byte(FILE* fp)
{
	char str[3];
	long l;
	str[0] = fgetc(fp);
	str[1] = fgetc(fp);
	str[2] = '\0';
	l = strtol(str, NULL, 16);
	return (Byte)(l & 0xff);
}
static Word fio_fread_hex_word(FILE* fp)
{
	Word ret;
	ret = fio_fread_hex_byte(fp);
	ret <<= 8;
	ret |= fio_fread_hex_byte(fp);
	return ret;
}
void FileIO::load_hex(const char* filename)
{
	bus->write(FIO_ERR_FLAGS, 0);
	if (strlen(filename) == 0)
	{
		// file not found
		Byte data = bus->read(FIO_ERR_FLAGS);
		data |= 0x80;
		bus->write(FIO_ERR_FLAGS, data);
		return;
	}

	// strip out \" characters
	std::string strFilename = "";
	for (int t = 0; t < strlen(filename); t++)
		if (filename[t] != '\"')
			strFilename += filename[t];
	FILE* fp;
	int done = 0;
	//setFilename(filename);

#pragma warning(suppress : 4996)
	fp = fopen(strFilename.c_str(), "r");
	if (!fp) {
		Byte data = bus->read(FIO_ERR_FLAGS);
		data |= 0x80;		// set the "File Not Found" bit
		bus->write(FIO_ERR_FLAGS, data);
		return;
	}

	while (!done) {
		Byte n, t;
		Word addr;
		Byte b;

		// (void)fgetc(fp);			// is this the : character at the start of the line?
		int colon = fgetc(fp);			// is this the : character at the start of the line?
		if (colon != ':')
		{
			printf("Wrong File Type\n");
			Byte data = bus->read(FIO_ERR_FLAGS);
			data |= 0x10;		
			bus->write(FIO_ERR_FLAGS, data);
			fclose(fp);
			return;
		}

		n = fio_fread_hex_byte(fp);
		addr = fio_fread_hex_word(fp);
		t = fio_fread_hex_byte(fp);
		if (t == 0x00) {
			while (n--) {
				b = fio_fread_hex_byte(fp);
				bus->debug_write(addr, b);
				++addr;
			}
		}
		else if (t == 0x01) {
			done = 1;
		}
		// Read and discard checksum byte
		(void)fio_fread_hex_byte(fp);
		if (fgetc(fp) == '\r') (void)fgetc(fp);
	}
	fclose(fp);
}



//// COMMAND FUNCTIONS ////////////////////

// $00 = Reset/Null
void FileIO::_cmd_reset()
{
	printf("FileIO::_cmd_reset()\n");
	bus->m_cpu->reset();
}

// $01 = Open/Create Binary File for Reading
void FileIO::_cmd_open_read()
{
	printf("FileIO::_cmd_open_read()\n");
}

// $02 = Open/Create Binary File for Writing
void FileIO::_cmd_open_write()
{
	printf("FileIO::_cmd_open_write()\n");
}

// $03 = Open/Create Binary File for Append
void FileIO::_cmd_open_append()
{
	printf("FileIO::_cmd_open_append()\n");
}

// $04 = Close File
void FileIO::_cmd_close()
{
	printf("FileIO::_cmd_close()\n");
}

// $05 = Read Byte
void FileIO::_cmd_read_byte()
{
	printf("FileIO::_cmd_read_byte()\n");
}

// $06 = Write Byte
void FileIO::_cmd_write_byte()
{
	printf("FileIO::_cmd_write_byte()\n");
}

// $07 = Load Hex Format File
void FileIO::_cmd_load_hex()
{
	printf("FileIO::_cmd_load_hex(\"%s\")\n", _filepath);

	// EXEC_VECTOR = $0010
	load_hex(_filepath);
}

// $08 = Write Hex Format Line
void FileIO::_cmd_write_hex_line()
{
	printf("FileIO::_cmd_write_hex_line()\n");
}

// $09 = Get File Length (FIO_BFRLEN = file length
void FileIO::_cmd_get_file_length()
{
	printf("FileIO::_cmd_get_file_length()\n");
}

// $0A = Load Binary File (read into FIO_BFROFS - FIO_BFROFS+FIO_BFRLEN)
void FileIO::_cmd_load_binary()
{
	printf("FileIO::_cmd_load_binary()\n");
}

// $0B = Save Binary File (wrote from FIO_BFROFS to FIO_BFROFS+FIO_BFRLEN)
void FileIO::_cmd_save_binary()
{
	printf("FileIO::_cmd_save_binary()\n");
}


// $0C = (not yet designed) List Directory
void FileIO::_cmd_list_dir()
{
	printf("FileIO::_cmd_list_dir()\n");

	printf("FILEPATH: %s\n", _filepath);
	
	// presumptions about _filepath  (*.cpp):
	//		if first character is a '*' then files are wild
	//      if last character is a '*' then extensions are wild
	//

	_files.clear();
	// std::filesystem::current_path("../");  // change dir
	std::string path = std::filesystem::current_path().generic_string();

	printf("path: %26s\n", path.c_str());
	std::string dirof = "DIR: " + path + "\n";
	_files.push_back(dirof);

	if (strlen(_filepath)==0)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::stringstream dir;
			if (entry.is_directory())
			{
				dir << "[" << entry.path().filename().generic_string() << "]";
				std::string stDir = dir.str();
				while (stDir.size() < 16)
					stDir = stDir + " ";
				_files.push_back(stDir);
			}
			if (entry.is_regular_file())
			{
				dir << entry.path().filename().generic_string();
				std::string stDir = dir.str();
				while (stDir.size() < 16)
					stDir = stDir + " ";
				_files.push_back(stDir);
			}
		}
	}
	else
	{
		std::string fp = _filepath;
		std::string file = "";
		std::string ext = "";
		int dot = 0;
		if (fp.at(0) == '*')
		{
			printf("WILD FILE\n");
			dot = fp.find('.');
			printf("dot: %d\n", dot);
			ext = "." + fp.substr(dot + 1);
			printf("EXT: %s\n", ext.c_str());
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				std::string strExt = entry.path().extension().filename().string();
				//printf("ext: %s == %s?\n", ext.c_str(), strExt.c_str());
				std::stringstream dir;
				if (entry.is_regular_file() && ext == strExt)
				{
					dir << entry.path().filename().generic_string();
					std::string stDir = dir.str();
					while (stDir.size() < 16)
						stDir = stDir + " ";
					_files.push_back(stDir);
				}
			}
		}
		if (fp.at(fp.size()-1) == '*')
		{
			printf("WILD EXTENSION\n");
			dot = fp.find('.');
			printf("dot: %d\n", dot);
			file = fp.substr(0, dot);
			printf("FILE: %s\n", file.c_str());
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				std::string strFile = entry.path().filename().string();
				
				strFile = strFile.substr(0, strFile.find('.'));
				printf("file: %s == %s?\n", file.c_str(), strFile.c_str());
				std::stringstream dir;
				if (entry.is_regular_file() && file == strFile)
				{
					dir << entry.path().filename().generic_string();
					std::string stDir = dir.str();
					while (stDir.size() < 16)
						stDir = stDir + " ";
					_files.push_back(stDir);
				}
			}
		}
	}
	// output the basic directory structure
	for (auto& f : _files)
		printf("%s\n", f.c_str());
}

// $0D = Make Directory
void FileIO::_cmd_make_dir()
{
	printf("FileIO::_cmd_make_dir()\n");
}

// $0E = Change Directory
void FileIO::_cmd_change_dir()
{
	printf("FileIO::_cmd_change_dir()\n");

	printf("FILEPATH: %s\n", _filepath);
	if (strlen(_filepath) == 0)		return;

	std::string chdir = _filepath;
	if (std::filesystem::exists(chdir))
	{
		//printf("Directory Found\n");
		Byte data = bus->read(FIO_ERR_FLAGS);
		data &= ~0x08;
		bus->write(FIO_ERR_FLAGS, data);
		_err_flags = data;
		std::filesystem::current_path(chdir);  // change dir
	}
	else
	{
		printf("ERROR: Directory Not Found!\n");
		Byte data = bus->read(FIO_ERR_FLAGS);
		data |= 0x08;
		bus->write(FIO_ERR_FLAGS, data);
		_err_flags = data;
	}

}

// $0F = Rename Directory
void FileIO::_cmd_rename_dir()
{
	printf("FileIO::_cmd_rename_dir()\n");
}

// $10 = Remove Directory
void FileIO::_cmd_remove_dir()
{
	printf("FileIO::_cmd_remove_dir()\n");
}

// $11 = Delete File
void FileIO::_cmd_delete_file()
{
	printf("FileIO::_cmd_delete_file()\n");
}

// $12 = Rename file
void FileIO::_cmd_rename_file()
{
	printf("FileIO::_cmd_rename_file()\n");
}

// $13 = Copy File
void FileIO::_cmd_copy_file()
{
	printf("FileIO::_cmd_copy_file()\n");
}
// $14 = Seek Start
void FileIO::_cmd_seek_start()
{
	printf("FileIO::_cmd_seek_start()\n");
}

// $15 = Seek Current
void FileIO::_cmd_seek_current()
{
	printf("FileIO::_cmd_seek_current()\n");
}

// $16 = Seek End
void FileIO::_cmd_seek_end()
{
	printf("FileIO::_cmd_seek_end()\n");
}








// * FileIO.cpp ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		Implementing File Input / Output
// *		with the onboad flash ram and
// *		the SDCARD
// ************************************

#include "types.h"
#include "Bus.h"
#include "Memory.h"
#include "FileIO.h"

//     FIO_BEGIN = 0x181e,        // start of file i/o hardware registers
//  FIO_ERR_FLAGS = 0x181e,       // (Byte) file i/o system flags:
//                                //      bit 7:  file not found
//                                //      bit 6:  end of file
//                                //      bit 5:  buffer overrun
//                                //      bit 0-4: not yet assigned
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
				else if (ofs == FIO_BFROFS+1)	data = (ptrFile->_buffer_offset & 0xFF00) | (data << 0);
				else if (ofs == FIO_BFRLEN)		data = (ptrFile->_buffer_length & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFRLEN + 1)	data = (ptrFile->_buffer_length & 0xFF00) | (data << 0);
				else if (ofs == FIO_SEEKOFS)		data = (ptrFile->_seek_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_SEEKOFS + 1)	data = (ptrFile->_seek_offset & 0xFF00) | (data << 0);

				else if (ofs >= FIO_FILEPATH && ofs < FIO_END)
				{
					Byte i = (ofs - FIO_FILEPATH) % 256;
					data = ptrFile->_filepath[i];
				}

				bus->debug_write(ofs, data);	// update the memory for debug_reads
			}
			else
			{	// WRITE TO
				if (ofs == FIO_ERR_FLAGS)	ptrFile->_err_flags = data;
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
					case 0x07:	ptrFile->_cmd_load_hex();				break;
					case 0x08:	ptrFile->_cmd_write_hex_line();			break;
					case 0x09:	ptrFile->_cmd_get_file_length();		break;
					case 0x0A:	ptrFile->_cmd_load_binary();			break;
					case 0x0B:	ptrFile->_cmd_save_binary();			break;
					case 0x0C:	ptrFile->_cmd_list_diry();				break;
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

					default:
						break;
					}
				}
				else if (ofs == FIO_BFROFS)		
					ptrFile->_buffer_offset = (ptrFile->_buffer_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFROFS+1)
					ptrFile->_buffer_offset = (ptrFile->_buffer_offset & 0xFF00) | (data << 0);

				else if (ofs == FIO_BFRLEN)
					ptrFile->_buffer_length = (ptrFile->_buffer_length & 0x00FF) | (data << 8);
				else if (ofs == FIO_BFRLEN + 1)
					ptrFile->_buffer_length = (ptrFile->_buffer_length & 0xFF00) | (data << 0);

				else if (ofs == FIO_SEEKOFS)
					ptrFile->_seek_offset = (ptrFile->_seek_offset & 0x00FF) | (data << 8);
				else if (ofs == FIO_SEEKOFS + 1)
					ptrFile->_seek_offset = (ptrFile->_seek_offset & 0xFF00) | (data << 0);

				else if (ofs >= FIO_FILEPATH && ofs < FIO_END)
				{
					Byte i = (ofs - FIO_FILEPATH) % 256;
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
	memmap->push({ offset, "", ">    bit 0-4: not yet assigned                 " }); offset += 0;

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

	memmap->push({ offset, "FIO_HANDLE", "(Byte) file handle or ZERO          " }); offset += 1;
	memmap->push({ offset, "FIO_BFROFS", "(Word) start of I/O buffer          " }); offset += 1;
	memmap->push({ offset, "FIO_BFRLEN", "(Word) length of I/O buffer         " }); offset += 1;
	memmap->push({ offset, "FIO_SEEKOFS","(Word) seek offset                  " }); offset += 1;
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

	//Byte d = bus->read(FIO_FLAGS);
	//d++;
	//bus->write(FIO_FLAGS, d);
	//printf("FileIO::OnInitialize() =--> Read(FIO_ERR_FLAGS): %d\n", bus->read(FIO_FLAGS));	
	//d++;
	//bus->write(FIO_FLAGS, d);
	//printf("FileIO::OnInitialize() =--> Read(FIO_ERR_FLAGS): %d\n", bus->read(FIO_FLAGS));	
	//d++;
	//bus->write(FIO_FLAGS, d);
	//printf("FileIO::OnInitialize() =--> Read(FIO_ERR_FLAGS): %d\n", bus->read(FIO_FLAGS));	
	//d++;
	//bus->write(FIO_FLAGS, d);
	//printf("FileIO::OnInitialize() =--> Read(FIO_ERR_FLAGS): %d\n", bus->read(FIO_FLAGS));
}

void FileIO::OnEvent(SDL_Event* evnt) {}
void FileIO::OnCreate() {}
void FileIO::OnDestroy() {}
void FileIO::OnUpdate(float fElapsedTime) {}
void FileIO::OnQuit() {}



//// COMMAND FUNCTIONS ////////////////////

// $00 = Reset/Null
void FileIO::_cmd_reset()
{
	printf("FileIO::_cmd_reset()\n");
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
	printf("FileIO::_cmd_load_hex()\n");
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
void FileIO::_cmd_list_diry()
{
	printf("FileIO::_cmd_list_diry()\n");
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


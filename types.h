// types.h
//
//  Include the correct SDL2/SDL.h file according to machine type.
//	Define unsigned 8-bit Byte and 16-bit Word types.
//
#pragma once
#ifndef __DEP_SDL_H__
#define __DEP_SDL_H__


const bool OUTPUT_MEMORY_MAP = false;	// display the memory map for copy/paste
const bool MEMORY_MAP_CPP = true;		// display memory map as C++ not ASM



#ifdef LINUX
	#include <SDL2/SDL.h>
#else
	#pragma comment(lib, "SDL2.lib")
	#pragma comment(lib, "SDL2main.lib")
	#include <SDL.h>
#endif

#ifndef __DEFINED_BYTE__
	#define __BYTE__
	typedef Uint8 Byte;
#endif
#ifndef __DEFINED_WORD__ 
	#define __DEFINED_WORD__
	typedef Uint16 Word;
#endif

#ifndef __DEFINED_DWORD__ 
	#define __DEFINED_DWORD__
	typedef uint32_t	DWord;
#endif

#endif // __DEP_SDL_H__
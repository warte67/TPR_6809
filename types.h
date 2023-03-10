// types.h
//
//  Include the correct SDL2/SDL.h file according to machine type.
//	Define unsigned 8-bit Byte and 16-bit Word types.
//
// * Copyright (C) 2023 by Jay Faries
#pragma once
#ifndef __DEP_SDL_H__
#define __DEP_SDL_H__

const bool OUTPUT_MEMORY_MAP		= false;	// display the memory map for copy/paste into MemoryMap.h and mem_map.asm
const bool MEMORY_MAP_CPP			= true;		// display memory map definition as C++ not ASM
const bool MEMORY_MAP_BLOCK_DUMP	= false;	// display block memory map


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
typedef uint32_t DWord;
#endif


const bool ENABLE_DEBUG = false;
const bool DEBUG_SINGLE_STEP = ENABLE_DEBUG;	// false;
const Byte DEBUG_BACKGROUND_ALPHA = 0xAA;	// alpha level of the debug screen background
const bool ENABLE_VSYNC = false;
const bool ENABLE_BACKBUFFER = false;
const bool DEFAULT_FULLSCREEN = true;
const int  DEFAULT_MONITOR = 1;
const Byte DEFAULT_MOUSE_SIZE = 8;

//const int  DEFAULT_GRAPHICS_MODE = 1; // TODO: Replace with DEFAULT_FG_GMODE and DEFAULT_BG_GMODE

const int DEFAULT_FG_GMODE = 1;
const int DEFAULT_BG_GMODE = 0;

#endif // __DEP_SDL_H__
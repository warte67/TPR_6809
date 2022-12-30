// dep_SDL.h
//
//   include the correct SDL2/SDL.h file according to machine type
//
#pragma once
#ifndef __DEP_SDL_H__
#define __DEP_SDL_H__


#ifdef LINUX
	#include <SDL2/SDL.h>
#else
	#pragma comment(lib, "SDL2.lib")
	#pragma comment(lib, "SDL2main.lib")
	#include <SDL.h>
#endif

#endif // __DEP_SDL_H__
// * GFX.h ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		running an R4G4B4 VGA resistor ladder DAC
// *		circuit for graphic display.
// ************************************

#pragma once
#ifndef __GFX_H__
#define __GFX_H__

#include "types.h"
#include "Device.h"

class GFX : public Device
{
    friend class Bus;

public:
    GFX();
    virtual ~GFX();

    virtual Word MapDevice(MemoryMap* memmap, Word offset);		// map this devices hardware registers

    virtual void OnInitialize();				// runs once after all devices are created
    virtual void OnEvent(SDL_Event *evnt);		// fires per SDL_Event
    virtual void OnCreate();					// fires when the object is created/recreated
    virtual void OnDestroy();					// fires when the object is destroyed/lost focus
    virtual void OnUpdate(float fElapsedTime);	// fires each frame, for updates
    virtual void OnRender();					// render the current frames texture
    virtual void OnQuit();						// fires on exit -- reverses OnInitialize()

protected:
	SDL_Window *_window = nullptr;
	SDL_Surface *_surface = nullptr;
	SDL_Renderer *_renderer = nullptr;
    SDL_Texture* _texture = nullptr;

	int _window_width = 0;
	int _window_height = 0;
	int _res_width = 256;
	int _res_height = 160;
	float _aspect = 1.6f;
};

#endif // __GFX_H__

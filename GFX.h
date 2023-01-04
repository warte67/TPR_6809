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
#include "Memory.h"

class GFX : public REG   // ToDo: Inherit from class Memory instead
{
    friend class Bus;

public:
    GFX();
    GFX(Word offset, Word size);
    virtual ~GFX();

    static Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);

    virtual Word MapDevice(MemoryMap* memmap, Word offset) override;

    virtual void OnInitialize();				// runs once after all devices are created
    virtual void OnEvent(SDL_Event *evnt);		// fires per SDL_Event
    virtual void OnCreate();					// fires when the object is created/recreated
    virtual void OnDestroy();					// fires when the object is destroyed/lost focus
    virtual void OnUpdate(float fElapsedTime);	// fires each frame, for updates
    //virtual void OnRender();					// render the current frames texture
    virtual void OnQuit();						// fires on exit -- reverses OnInitialize()

protected:

    void _onRender();   // render the GFX children (graphics modes)

	SDL_Window *_window = nullptr;
	SDL_Surface *_surface = nullptr;
	SDL_Renderer *_renderer = nullptr;
    SDL_Texture* _texture = nullptr;

    // default screen size
    const float _aspect = 1.6f;
	const int _window_width = 512 * 3;        // aka "timing"
	const int _window_height = int(float((_window_width) + 0.5f) / _aspect);
	int _res_width = 512;			// 512 width max
	int _res_height = int(float((_res_width) + 0.5f) / _aspect);
    const int _window_flags = SDL_WINDOW_SHOWN;	// | SDL_WINDOW_BORDERLESS; // | SDL_WINDOW_RESIZABLE;
    const int _fullscreen_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    Uint32 _renderer_flags = 0;

    // statics
    static bool m_VSYNC;		// 1:VSYNC, 2:not throttled
    static bool m_fullscreen;	// 1:fullscreen, 2:windowed
    static int  m_display_num;	// which monitor to use

    // internal state
    //bool bWasInit = false;
    bool bIsDirty = true;


    // device objects
    Bus* bus = nullptr;
    Memory* memory = nullptr;
};




#endif // __GFX_H__


/***********************************************

    Screen Timing Notes:

    Screen Resolution is based on a maximum 512x320 pixel timing.
        - Text / Glyph Resolutions:
            - 512x320 x 16-Color (only)
                - Glyph Data Buffer size = 2.5kb
                - Color Attribute Data Buffer size = 2.5kb
        - Possible Pixel Graphics Resolutions include:
            - 256x160 x 2-Color
            - 256x80 x 4-Color
            - 128x160 x 4-Color
            - 128x80 x 16-Color

************************************************/
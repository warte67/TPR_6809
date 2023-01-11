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

class GfxMode;

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
	virtual void OnQuit();						// fires on exit -- reverses OnInitialize()
	virtual void OnEvent(SDL_Event *evnt);		// fires per SDL_Event
    virtual void OnCreate();					// fires when the object is created/recreated
    virtual void OnDestroy();					// fires when the object is destroyed/lost focus
    virtual void OnUpdate(float fElapsedTime);	// fires each frame, for updates
    //virtual void OnRender();					// render the current frames texture
 
	// accessors:
	int PixHeight() { return _pix_height; }
	int PixWidth() { return _pix_width; }
	SDL_Renderer* Renderer() { return _renderer; }

protected:

    void _onRender();   // render the GFX children (graphics modes)

	SDL_Window *_window = nullptr;
	SDL_Surface *_surface = nullptr;
	SDL_Renderer *_renderer = nullptr;
    SDL_Texture* _texture = nullptr;
    //SDL_Texture* _texture[2] = { nullptr, nullptr };    // two backbuffer textures

    // default screen size
    const float _aspect = 1.6f; 
	const int _window_width = 512 * 3;                                          // aka "timing"
	const int _window_height = int(float((_window_width) + 0.5f) / _aspect);    // aka "timing"
	int _pix_width = 512;			// 512 width max
	int _pix_height = int(float((_pix_width) + 0.5f) / _aspect);
    const int _window_flags = SDL_WINDOW_SHOWN;	// | SDL_WINDOW_BORDERLESS; // | SDL_WINDOW_RESIZABLE;
    const int _fullscreen_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    Uint32 _renderer_flags = 0;

    // statics (hardware)
    static bool m_VSYNC;		        // 1:VSYNC, 0:not throttled
    static bool m_enable_backbuffer;    // 1:enabled, 0:disabled
    static bool m_enable_debug;         // 1:enabled, 0:disabled
    static bool m_enable_mouse;         // 1:enabled, 0:disabled
    static int  m_current_backbuffer;   // currently active backbuffer
    static int  m_gmode_index;          // (0-7)
    // statics (auxillary)
    static bool m_fullscreen;	        // 1:fullscreen, 0:windowed
    static int  m_display_num;	        // which monitor to use

    // internal state
    //bool bWasInit = false;
    bool bIsDirty = true;

    // graphics modes    
	std::vector<GfxMode*> m_gmodes;

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
                - (5120 bytes)
        - Possible Pixel Graphics Resolutions include:
            - 256x160 x 2-Color     (5120 bytes)
            - 256x80 x 4-Color      (5120 bytes)
            - 128x160 x 4-Color     (5120 bytes)
            - 128x80 x 16-Color     (5120 bytes)

	GFX_FLAGS = 0x1800,        // (Byte) gfx system flags:
		bit 7: fullscreen
		bit 6: vsync
		bit 3-5: display monitor (0-7)
		bit 0-2: graphics mode (0-7)
			0) GfxNull:		NONE (just random background noise)
			1) GfxGlyph:	Glyph Mode (512x320 or 64x48 text)
			2) GfxTile:		Tile 16x16x16 mode
			3) GfxBitmap1:	128x80 x 16-Color
			4) GfxBitmap2:	128x160 x 4-Color
			5) GfxBitmap3:	256x80 x 4-Color
			6) GfxBitmap4:	256x160 x 2-Color
			7) GfxBitmap5:	256x192 256-color (SLOW EXTERNAL I2C RAM)	

	TIMING_WIDTH = 0x1801,        // (Word) timing width
	TIMING_HEIGHT = 0x1803,       // (Word) timing height

	STATIC MODES:
		+ DEBUG
		+ SPRITES
		+ SYSTEM (Mouse Cursor)

Revision ///////////////////

    GFX_FLAGS: (hardware)
        bits:
        6)   VSYNC
        6)   backbuffer enable
        5)   enable debug
        4)   enable mouse cursor
        3)   swap backbuffer (on write); current backbuffer (on read)
        0-2) graphics mode index

    GFX_AUX: (emulator only)
        bits:
        7)   0:fullscreen / 1:windowed
        6)   reserved
        5)   reserved
        4)   reserved
        3)   reserved
        0-2  display index

************************************************/
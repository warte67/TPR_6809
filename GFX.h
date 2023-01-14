// * Gfx.h ***************************************
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
class GfxDebug;
class GfxMouse;

class GFX : public REG   // ToDo: Inherit from class Memory instead
{
    friend class Bus;
    friend class GfxMouse;
    friend class GfxGlyph64;
    friend class GfxGlyph32;
    friend class GfxTile;
    friend class GfxBmp16;
    friend class GfxBmp2;
    friend class GfxRaw;
    friend class GfxHires;

public:
    GFX();
    GFX(Word offset, Word size);
    virtual ~GFX();

    static Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);

    virtual Word MapDevice(MemoryMap* memmap, Word offset) override;
    Word MapDevices(MemoryMap* memmap, Word offset);

    virtual void OnInitialize();				// runs once after all devices are created
	virtual void OnQuit();						// fires on exit -- reverses OnInitialize()
	virtual void OnEvent(SDL_Event *evnt);		// fires per SDL_Event
    virtual void OnCreate();					// fires when the object is created/recreated
    virtual void OnDestroy();					// fires when the object is destroyed/lost focus
    virtual void OnUpdate(float fElapsedTime);	// fires each frame, for updates
    //virtual void OnRender();					// render the current frames texture
 
	// accessors:
    int PixWidth() { return _pix_width; }
    int PixHeight() { return _pix_height; }
    int WindowWidth() { return _window_height; }
    int WindowHeight() { return _window_width; }
    int ResWidth() { return _res_width; }
    int ResHeight() { return _res_height; }
    SDL_Renderer* Renderer() { return _renderer; }
    SDL_Texture* Texture() { return _texture[m_current_backbuffer]; }
    SDL_Window* Window() { return _window; }
    float Aspect() { return _aspect; }
    bool Fullscreen() { return m_fullscreen; }
    int DisplayNum() { return m_display_num; }
    bool MouseEnabled() { return m_enable_mouse; }
    void MouseEnabled(bool en) { m_enable_mouse = en; }
    bool DebugEnabled() { return m_enable_debug; }
    void DebugEnabled(bool en) { m_enable_debug = en; }


protected:

    void _onRender();   // render the GFX children (graphics modes)

	SDL_Window *_window = nullptr;
	SDL_Surface *_surface = nullptr;
	SDL_Renderer *_renderer = nullptr;
    //SDL_Texture* _texture = nullptr;
    SDL_Texture* _texture[2] = { nullptr, nullptr };    // two backbuffer textures

    // default screen size
    const float _aspect = 1.6f; 
	const int _window_width = 512 * 3;                                          // aka "timing"
	const int _window_height = int(float((_window_width) + 0.5f) / _aspect);    // aka "timing"
	const int _pix_width = 512;			// 512 width max
	const int _pix_height = int(float((_pix_width) + 0.5f) / _aspect);
    int _res_width = 512;
    int _res_height = int(float((_res_width) + 0.5f) / _aspect);
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
    GfxMouse* gfx_mouse = nullptr;
    GfxDebug* gfx_debug = nullptr;

    // Palette Stuff
    union PALETTE {
        Word color;
        struct {
            Uint8 a : 4;
            Uint8 b : 4;
            Uint8 g : 4;
            Uint8 r : 4;
        };
    };
    std::vector<PALETTE> palette;
    static Uint8 m_palette_index;

public:
    Uint8 red(Uint8 index)      { Uint8 c = palette[index].r;  return c | (c << 4); }
    Uint8 grn(Uint8 index)      { Uint8 c = palette[index].g;  return c | (c << 4); }
    Uint8 blu(Uint8 index)      { Uint8 c = palette[index].b;  return c | (c << 4); }
    Uint8 alf(Uint8 index)      { Uint8 c = palette[index].a;  return c | (c << 4); }
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
			1) GfxGlyph32:	Glyph Mode (256x160 or 32x20 text)
            2) GfxGlyph64:	Glyph Mode (512x320 or 64x40 text)
            3) GfxTile:		Tile 16x16x16 mode
			4) GfxBmp16:	128x80 x 16-Color
			5) GfxBmp2:	    256x160 x 2-Color
            6) GfxRaw:      128x80 x 4096-Color (16 bpp 20KB) - Serial Buffer / FPGA
            7) GfxHires:    512x320 x 2-Color (1 bpp 20KB) - Serial Buffer / FPGA

            What if GfxBmp2 (256x160 Mode 7) had a special 64-color (RRGGBBXX) or 256-color (RRGGBBII)
                    that uses an external 40k (256x160 = 40960) buffer?
                - What to do with the stock 5120 (5KB) byte video buffer memory?
                - What to do with the remaining 16KB of an external 64KB RAM chip?
                - Is it likely implementation on a PICO would be a pain-in-the-arse? (probably so)
			    - GfxBmpExt:	256x192 256-color (SLOW EXTERNAL I2C RAM)	
                - Maybe leave this mode as an option for for later, but for now, DON'T DO IT!



	STATIC MODES:
		+ DEBUG
		+ SPRITES (What about priority display layers?)
		+ SYSTEM (Mouse Cursor)

Revision Notes ///////////////////

    GFX_FLAGS: (hardware)
        bits:
        7)   VSYNC
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



    Color Palette Entry:
        +-+-+-+-+-+-+-+-+
        |R|R|G|G|B|B|A|A|   64-Color Palette with ALPHA BLENDING
        +-+-+-+-+-+-+-+-+

    "256-Color" Palette Entry:
        +-+-+-+-+-+-+-+-+
        |R|R|G|G|B|B|I|I|    Uses 2 Intensity Bits instead of Alpha
        +-+-+-+-+-+-+-+-+


//// OBSERVATION Notes ///////////////////

        64-colors (RRGGBBAA) = yuck 
            pros:
                - only uses 6 pins on the PICO hardware
                - fewer bits required to store color data
                 - color data fits nicely witin a single byte
            cons:
                - no equivalent native SDL Pixel Format (SLOWER - software rendering ONLY)
                - very ugly and restricted color palette               

        4096 color (RRRRGGGGBBBBAAAA) = acceptable (12-pins on the PICO)
            pros:
                - FASTER - direct hardware mapping to native SDL Pixel Format (SDL_PIXELFORMAT_RGBA4444)
                - Color Palette is comprehensive
            cons: 
                - color data requires two bytes per palette entry
                - uses 12 pins on the PICO (in addition to VSYNC, HSYNC, and PIXELCLOCK)


    
        The 4-Color modes are very ugly with non-square pixels. It would be best to restrict
            display resolutions to be fixed at a single aspect ratio (using 16/10).

                Replace with:

                    0) GfxNull:		NONE (just random background noise)
                    1) GfxGlyph32:	Glyph Mode (256x160 or 32x20 text)   1280 bytes each (4 layers)
                    2) GfxGlyph64:	Glyph Mode (512x320 or 64x40 text)   5120 bytes each (1 layer)
                    3) GfxTile16:   Tile 16x16 x 256 colors in 256x160 screen bitmap   
                    4) GfxBmp16:	128x80 x 16-Color (5120)
                    5) GfxBmp2:	    256x160 x 2-Color (5120)
                    6) GfxRaw:      128x80 x 4096-Color (16 bpp 20KB) - Serial Buffer / FPGA
                    7) GfxHires:    512x320 x 2-Color (1 bpp 20KB) - Serial Buffer / FPGA



************************************************/
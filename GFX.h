/**** GFX.h ***************************************
 *
 *   Acts to emulate a Raspberry PI Pico
 *		running an R4G4B4 VGA resistor ladder DAC
 *		circuit for graphic display.
 *
 * Copyright (C) 2023 by Jay Faries
 ************************************/
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
    friend class GfxTile16;
    friend class GfxTile32;
    friend class GfxBmp16;
    friend class GfxBmp2;
    friend class GfxIndexed;
    friend class GfxRaw;

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
    //bool MouseEnabled() { return m_enable_mouse; }
    //void MouseEnabled(bool en) { m_enable_mouse = en; }
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
    // static bool m_enable_mouse;         // 1:enabled, 0:disabled
    static int  m_current_backbuffer;   // currently active backbuffer

    // TODO:  change m_gmode_index to m_fg_mode_index and m_bg_mode_index
    //static int  m_gmode_index;          // (0-7)
    int m_fg_mode_index = DEFAULT_FG_GMODE;
    int m_bg_mode_index = DEFAULT_BG_GMODE;

    // statics (auxillary)
    static bool m_fullscreen;	        // 1:fullscreen, 0:windowed
    static int  m_display_num;	        // which monitor to use

    // internal state
    //bool bWasInit = false;
    bool bIsDirty = true;
    bool bRebuildTextures = false;      // true when the gmode textures need redrawn

    // graphics modes    
	//std::vector<GfxMode*> m_gmodes;
    std::vector<GfxMode*> m_bg_gmodes;
    std::vector<GfxMode*> m_fg_gmodes;

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
    Uint8 red(Uint8 index) { Uint8 c = palette[index].r;  return c | (c << 4) | (c << 8) | (c << 12); }
    Uint8 grn(Uint8 index) { Uint8 c = palette[index].g;  return c | (c << 4) | (c << 8) | (c << 12); }
    Uint8 blu(Uint8 index) { Uint8 c = palette[index].b;  return c | (c << 4) | (c << 8) | (c << 12); }
    Uint8 alf(Uint8 index) { Uint8 c = palette[index].a;  return c | (c << 4) | (c << 8) | (c << 12); }

private:
    Byte _clock_div = 0;
    Byte clock_div(Byte& cl_div, int bit);
    void clockDivider();
};




#endif // __GFX_H__


/***********************************************
*   NOTES:
************************************** 

    TODO: 
        - Once again, convert back to working with RGBA4444 format
            - RGBA2222 format works but 64 color wastes 2 bits per pixel.
            - 256-Color Indexed mode artwork is much easier to create and save.
            - There should be plenty of pins on PICO #1.

	GFX_FLAGS = 0x1800,        // (Byte) gfx system flags:
		bit 7: VSYNC
		bit 6: backbuffer enable
        bit 5: swap backbuffers (on write)
        bit 4: debug enable
        bits 2-3 = "Background" graphics mode (20KB buffer)
                0) GfxNull()        NONE (forced black background)
                1) GfxTile16()      Tile 16x16 mode             
                2) GfxTile32()      Overscan Tile 16x16 mode    
                3) GfxIndexed()     128x80 x 256-Colors (2 x 10K buffers)

        bits 0-1 = "Foreground" graphics mode (5KB buffer)
                0) GfxBmp2()        256x160 x 2-Color
                1) GfxGlyph32()     Glyph Mode (256x160 or 32x20 text)
                2) GfxGlyph64()     Glyph Mode (512x320 or 64x40 text)
                3) GfxBmp16()       128x80 x 16-Color

    GFX_AUX: (emulator only)
        bits:
        7)   0:fullscreen / 1:windowed
        6)   reserved
        5)   reserved
        4)   reserved
        3)   reserved
        0-2  display index

    ************************************************************

    Maths:

    64KB External Ram Buffer:
        GfxIndexed # 1 - 10k: $0000-$27ff        = 128x80  x 256-color screen (10 kb each)
        GfxIndexed # 2 - 10k: $2800-$4fff        = 128x80  x 256-color screen (10 kb each)
        TILES x 112    - 28k: $5000-$BFFF        = 16x16 x 256-color Tiles    (256 bytes each)
        SPRITES x 64   - 16k: $C000-$FFFF        = 16x16 x 256-color Sprites  (256 bytes each)

    STATIC MODES:
		+ DEBUG 
		+ SPRITES (What about priority display layers?)
		+ SYSTEM (Mouse Cursor)     


    ************************************************************


    Raspberry PI PICO has 26 Multifunction GPIO Pins:
        - 1 x LED (used internally, not available externally?)
        - 3 x Analogue Inputs
        - 2 x UART controllers
        - 2 x SPI controllers (1 used for communicatoin with the other PICO)
        - 2 x I2C controllers, 
        - 16 x PWM channels 

    Raspberry PI PICO #1 GPIO Pins in consideration:
        - 1 SPI controller to communicate I/O events with the other PICO
        - 3 x Analogue Inputs
            - FIRQ
            - IRQ
            - NMI
        - 16 x PWM channels
            - 12 used for color data
            - 1 VSYNC
            - 1 HSYNC
            - 1 system clock
            - 1 system reset
        
    Raspberry PI PICO #2 GPIO Pins in consideration:
        - 1 SPI controller to communicate I/O events with the other PICO
        - 2 Stereo Audio outs
        - 6 QSPI I/O with SDRAM flash card
        - 1 UART USB Keyboard, Mouse, Gamepad #1, Gamepad #2
        - 1 UART Serial Terminal I/O (and serial debugging)



************************************************/
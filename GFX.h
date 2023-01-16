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
    friend class GfxTile16;
    friend class GfxTile32;
    friend class GfxBmp16;
    friend class GfxBmp2;
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
        Byte color;
        struct {
            Uint8 a : 2;
            Uint8 b : 2;
            Uint8 g : 2;
            Uint8 r : 2;
        };
    };
    std::vector<PALETTE> palette;
    static Uint8 m_palette_index;

public:
     Uint8 red(Uint8 index) { Uint8 c = palette[index].r;  return c | (c << 2) | (c << 4) | (c << 6); }
     Uint8 grn(Uint8 index) { Uint8 c = palette[index].g;  return c | (c << 2) | (c << 4) | (c << 6); }
     Uint8 blu(Uint8 index) { Uint8 c = palette[index].b;  return c | (c << 2) | (c << 4) | (c << 6); }
     Uint8 alf(Uint8 index) { Uint8 c = palette[index].a;  return c | (c << 2) | (c << 4) | (c << 6); }
};




#endif // __GFX_H__


/***********************************************

	GFX_FLAGS = 0x1800,        // (Byte) gfx system flags:
		bit 7: VSYNC
		bit 6: backbuffer enable
        bit 5: swap backbuffers (on write)
        bit 4: debug enable
        bits 2-3 = "Background" graphics mode (40KB buffer)
                0) GfxNull()        NONE (forced black background)
                1) GfxTile16()      Tile 16x16 mode
                2) GfxTile32()      Overscan Tile 16x16 mode
                3) GfxRaw()         256x160 x 64-Colors (40k)   MAYBE: 160x80 x 64-Colors (10k)

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
    
    STATIC MODES:
		+ DEBUG
        + LABELS ("labels" are text based sprites)
		+ SPRITES (What about priority display layers?)
		+ SYSTEM (Mouse Cursor)


    ************************************************************
    
    Notes:
        GfxHires() is entirely TOO SLOW. I propose Renaming GfxTile() to GfxTile16() and
        add a new GfxTile32() which is simply a double scan version. 





    

    Raspberry PI PICO has 26 Multifunction GPIO Pins:
        - 1 x LED (used internally, not available externally?)
        - 3 x Analogue Inputs
        - 2 x UART controllers
        - 2 x SPI controllers
        - 2 x I2C controllers, 
        - 16 x PWM channels



        


************************************************/
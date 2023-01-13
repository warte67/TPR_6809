// * GFX.cpp ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		running an R4G4B4 VGA resistor ladder DAC
// *		circuit for graphic display.
// ************************************

#include "Bus.h"
#include "Memory.h"
#include "Device.h"
#include "GfxMode.h"
#include "GfxGlyph.h"
#include "GfxDebug.h"
#include "GfxMouse.h"
#include "GfxBmp16.h"
#include "GFX.h"


// default GFX_FLAGS:
bool GFX::m_VSYNC				= false;	// true:VSYNC, false:not throttled
bool GFX::m_enable_backbuffer	= false;	// true:enabled, false:disabled
bool GFX::m_enable_debug		= false;		// true:enabled, false:disabled
bool GFX::m_enable_mouse		= true;		// true:enabled, false:disabled
int  GFX::m_current_backbuffer	= 0;		// currently active backbuffer (0-1)
int  GFX::m_gmode_index			= 3;		// active graphics mode (0-7)

// default GFX_AUX:
bool GFX::m_fullscreen			= false;	// true:fullscreen, false:windowed
int  GFX::m_display_num			= 1;		// which monitor to use, default.

// Palette Index
Uint8 GFX::m_palette_index = 0;


Byte GFX::OnCallback(REG* memDev, Word ofs, Byte data, bool bWasRead)
{
	//printf("GFX::OnCallback()\n");
	Bus* bus = Bus::getInstance();

	GFX* ptrGfx = dynamic_cast<GFX*>(memDev);
	if (ptrGfx)
	{
		if (bWasRead)
		{	// READ FROM
			if (ofs == GFX_FLAGS)
			{	// READ:
				//		bit 7: vsync
				//      bit 6: backbuffer enable
				//      bit 5: debug enable
				//      bit 4: mouse cursor enable
				//      bit 3: swap backbuffers (on write)
				//      bit 0-2: graphics mode (0-7)
				//          0) NONE (just random background noise)
				//          1) Glyph Mode (512x320 or 64x48 text)
				//          2) Tile 16x16x16 mode
				//          3) 128x80 x 16-Color
				//          4) 128x160 x 4-Color
				//          5) 256x80 x 4-Color
				//          6) 256x160 x 2-Color
				//          7) 256x192 256-color  (EXTERNAL 64k BUFFER)
				Byte ret = 0;
				if (ptrGfx->m_VSYNC)				ret |= 0x80;
				if (ptrGfx->m_enable_backbuffer)	ret |= 0x40;
				if (ptrGfx->m_enable_debug)			ret |= 0x20;
				if (ptrGfx->m_enable_mouse)			ret |= 0x10;
				if (ptrGfx->m_current_backbuffer)	ret |= 0x08;
				ret |= (ptrGfx->m_gmode_index & 0x07);
				ptrGfx->write(ofs, ret);	// pre-write
				return ret;
			}
			if (ofs == GFX_AUX)
			{	// READ:
				//      bit 7: 1:fullscreen / 0:windowed
				//      bit 6: reserved
				//      bit 5: reserved
				//      bit 4: reserved
				//      bit 3: reserved
				//      bit 0-2: monitor display index (0-7)
				Byte ret = 0;
				if (ptrGfx->m_fullscreen)	ret |= 0x80;
				ret |= (ptrGfx->m_display_num & 0x07);
				ptrGfx->write(ofs, ret);	// pre-write
				return ret;
			}
			// All we care about here is the resolution width/height. This represents the
			//     screen timing resolution the PICO will have to display.
			if (ofs == TIMING_WIDTH)
				return (ptrGfx->_pix_width >> 8) & 0x00ff;
			if (ofs == TIMING_WIDTH + 1)
				return ptrGfx->_pix_width & 0x00ff;
			if (ofs == TIMING_HEIGHT)
				return (ptrGfx->_pix_height >> 8) & 0x00ff;
			if (ofs == TIMING_HEIGHT + 1)
				return ptrGfx->_pix_height & 0x00ff;

			// read PALETTE stuff
			if (ofs == GFX_PAL_INDX)
				return m_palette_index;
			if (ofs == GFX_PAL_DATA)
				return ptrGfx->palette[m_palette_index].color;
		}
		else
		{	// WRITTEN TO

			if (ofs == GFX_FLAGS)
			{	// WRITE:
				//		bit 7: vsync
				//      bit 6: backbuffer enable
				//      bit 5: debug enable
				//      bit 4: mouse cursor enable
				//      bit 3: swap backbuffers (on write)
				//      bit 0-2: graphics mode (0-7)
				//          0) NONE (just random background noise)
				//          1) Glyph Mode (512x320 or 64x48 text)
				//          2) Tile 16x16x16 mode
				//          3) 128x80 x 16-Color
				//          4) 128x160 x 4-Color
				//          5) 256x80 x 4-Color
				//          6) 256x160 x 2-Color
				//          7) 256x192 256-color (SLOW EXTERNAL I2C RAM)
				bool old_VSYNC = ptrGfx->m_VSYNC;
				static int old_gmode_index = 0;
				ptrGfx->m_VSYNC					= ((data & 0x80) == 0x80);
				ptrGfx->m_enable_backbuffer		= ((data & 0x40) == 0x40);
				ptrGfx->m_enable_debug			= ((data & 0x20) == 0x20);
				ptrGfx->m_enable_mouse			= ((data & 0x10) == 0x10);
				if (ptrGfx->m_enable_backbuffer)
					ptrGfx->m_current_backbuffer	= ((data & 0x08) == 0x08);
				else
					ptrGfx->m_current_backbuffer = 0;

				ptrGfx->m_gmode_index			= (data & 0x07);
				//

				// only go "dirty" on VSYNC change
				if (old_VSYNC != ptrGfx->m_VSYNC)
					ptrGfx->bIsDirty = true;
				if (old_gmode_index != ptrGfx->m_gmode_index)
				{
					ptrGfx->m_gmodes[old_gmode_index]->OnDeactivate();
					ptrGfx->m_gmodes[ptrGfx->m_gmode_index]->OnActivate();
					// ptrGfx->bIsDirty = true;
				}
				old_gmode_index = ptrGfx->m_gmode_index;

				ptrGfx->debug_write(ofs, data);
			}
			if (ofs == GFX_AUX)
			{	// WRITE:
				//      bit 7: 0:fullscreen / 1:windowed
				//      bit 6: reserved
				//      bit 5: reserved
				//      bit 4: reserved
				//      bit 3: reserved
				//      bit 0-2: monitor display index (0-7)
				ptrGfx->m_fullscreen = ((data & 0x80) == 0x80);
				ptrGfx->m_display_num = (data & 0x07);
				int num = SDL_GetNumVideoDisplays();
				ptrGfx->m_display_num %= num;
				//
				ptrGfx->bIsDirty = true;
				ptrGfx->debug_write(ofs, data);
			}

			// write PALETTE index 
			if (ofs == GFX_PAL_INDX)
			{ 
				ptrGfx->debug_write(ofs, data); 
				m_palette_index = data; 
				bus->debug_write(GFX_PAL_DATA, ptrGfx->palette[m_palette_index].color);
			}
			if (ofs == GFX_PAL_DATA)
			{
				bus->debug_write(ofs, data);
				ptrGfx->palette[m_palette_index].color = data;
			}
		}
		// intercept for GfxMouse
		if (ofs >= CSR_XPOS && ofs <= CSR_BMP_DATA)
			return ptrGfx->gfx_mouse->OnCallback(memDev, ofs, data, bWasRead);
	}
	return data;
}



GFX::GFX() : REG(0,0)
{
	Device::_deviceName = "???GFX???";
	// this constructore is removed early.
	// dont use it for initialization
}
GFX::GFX(Word offset, Word size) : REG(offset, size)
{
	Device::_deviceName = "GFX";
	bus = Bus::getInstance();
	bus->m_gfx = this;
	memory = bus->getMemoryPtr();
	//// pre-build the graphics modes
	//for (int t = 0; t < 8; t++)
	//{
	//	GfxMode* mode = new GfxMode();
	//	m_gmodes.push_back(mode);
	//}
	//// replace the for loop above with:
	m_gmodes.push_back(new GfxNull());
	m_gmodes.push_back(new GfxGlyph());
	m_gmodes.push_back(new GfxMode());	// m_gmode.push_back(new GfxTile());
	m_gmodes.push_back(new GfxBmp16());
	m_gmodes.push_back(new GfxMode());	// m_gmode.push_back(new GfxGfxBitmap2();
	m_gmodes.push_back(new GfxMode());	// m_gmode.push_back(new GfxGfxBitmap3();
	m_gmodes.push_back(new GfxMode());	// m_gmode.push_back(new GfxGfxBitmap4();
	m_gmodes.push_back(new GfxMode());	// m_gmode.push_back(new GfxGfxBitmap5();

	// initialize GfxDebug
	if (gfx_debug == nullptr)
		gfx_debug = new GfxDebug();

	// initialize GfxSystem
	if (gfx_mouse == nullptr)
		gfx_mouse = new GfxMouse();
}
GFX::~GFX()
{    
	// clean up the graphics modes
	for (auto& a : m_gmodes)
		delete a;

	// Destroy gfx_Debug
	if (gfx_debug)
	{
		delete gfx_debug;
		gfx_debug = nullptr;
	}	// Destroy GfxSystem
	if (gfx_mouse)
	{
		delete gfx_mouse;
		gfx_mouse = nullptr;
	}
}


Word GFX::MapDevice(MemoryMap* memmap, Word offset)
{
	std::string reg_name = "GFX System";
	DWord st_offset = offset;
	// Defined only to serve as a template for inherited device objects.
	// (this will never be called due to being an abstract base type.)

	// map fundamental GFX hardware registers:
	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Graphics Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "GFX_FLAGS", "(Byte) gfx system flags:" }); offset += 1;
	memmap->push({ offset, "", ">    bit 7: vsync" }); offset += 0;
	memmap->push({ offset, "", ">    bit 6: backbuffer enable" }); offset += 0;
	memmap->push({ offset, "", ">    bit 5: debug enable" }); offset += 0;
	memmap->push({ offset, "", ">    bit 4: mouse cursor enable" }); offset += 0;
	memmap->push({ offset, "", ">    bit 3: swap backbuffers (on write)" }); offset += 0;
	memmap->push({ offset, "", ">    bit 0-2: graphics mode (0-7)" }); offset += 0;
	memmap->push({ offset, "", ">        0) NONE (just random background noise)		 " }); offset += 0;
	memmap->push({ offset, "", ">        1) Glyph Mode (512x320 or 64x40 text)		 " }); offset += 0;
	memmap->push({ offset, "", ">        2) Tile 16x16x16 mode						 " }); offset += 0;
	memmap->push({ offset, "", ">        3) 128x80 x 16-Color						 " }); offset += 0;
	memmap->push({ offset, "", ">        4) 128x160 x 4-Color						 " }); offset += 0;
	memmap->push({ offset, "", ">        5) 256x80 x 4-Color						 " }); offset += 0;
	memmap->push({ offset, "", ">        6) 256x160 x 2-Color						 " }); offset += 0;
	memmap->push({ offset, "", ">        7) 256x192 256-color RGBI2222 (64k BUFFER) " }); offset += 0;
	memmap->push({ offset, "GFX_AUX", "(Byte) gfx auxillary/emulation flags:" }); offset += 1;
	memmap->push({ offset, "", ">    bit 7: 1:fullscreen / 0:windowed" }); offset += 0;
	memmap->push({ offset, "", ">    bit 6: reserved" }); offset += 0;
	memmap->push({ offset, "", ">    bit 5: reserved" }); offset += 0;
	memmap->push({ offset, "", ">    bit 4: reserved" }); offset += 0;
	memmap->push({ offset, "", ">    bit 3: reserved" }); offset += 0;
	memmap->push({ offset, "", ">    bit 0-2: monitor display index (0-7)" }); offset += 0;
	memmap->push({ offset, "TIMING_WIDTH", "(Word) timing width" }); offset += 2;
	memmap->push({ offset, "TIMING_HEIGHT", "(Word) timing height" }); offset += 2;
	memmap->push({ offset, "GFX_PAL_INDX", "(Byte) gfx palette index (0-15)" }); offset += 1;
	memmap->push({ offset, "GFX_PAL_DATA", "(Byte) gfx palette color bits RRGGBBAA" }); offset += 1;

	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Mouse Cursor Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "CSR_XPOS", "(Word) horizontal mouse cursor coordinate" }); offset += 2;
	memmap->push({ offset, "CSR_YPOS", "(Word) vertical mouse cursor coordinate" }); offset += 2;
	memmap->push({ offset, "CSR_XOFS", "(Byte) horizontal mouse cursor offset" }); offset += 1;
	memmap->push({ offset, "CSR_YOFS", "(Byte) vertical mouse cursor offset" }); offset += 1;
	memmap->push({ offset, "CSR_SIZE", "(Byte) cursor size (0-15) 0:off" }); offset += 1;
	memmap->push({ offset, "CSR_SCROLL", "(Signed) MouseWheel Scroll: -1, 0, 1" }); offset += 1;
	memmap->push({ offset, "CSR_FLAGS", "(Byte) mouse button flags:" }); offset += 1;
	memmap->push({ offset, "", ">    bits 0-5: button states" }); offset += 0;
	memmap->push({ offset, "", ">    bits 6-7: number of clicks" }); offset += 0;
	memmap->push({ offset, "CSR_PAL_INDX", "(Byte) mouse cursor color palette index (0-15)" }); offset += 1;
	memmap->push({ offset, "CSR_PAL_DATA", "(Byte) mouse cursor color palette data RRGGBBAA" }); offset += 1;
	memmap->push({ offset, "CSR_BMP_INDX", "(Byte) mouse cursor bitmap pixel offset" }); offset += 1;
	memmap->push({ offset, "CSR_BMP_DATA", "(Byte) mouse cursor bitmap pixel color" }); offset += 1;

	memmap->push({ offset, "", "" }); offset -= 1;
	memmap->push({ offset, "GFX_END", "end of the GFX Hardware Registers" }); offset += 1;
	memmap->push({ offset, "", "" }); offset += 0;

	return offset - st_offset;
}

void GFX::OnInitialize() 
{
	//printf("Gfx::OnInitialize\n");

	if (palette.size() == 0)
	{
		for (int t = 0; t < 16; t++)
			palette.push_back({0x00});

		std::vector<PALETTE> ref = {
			{ 0x03 },	// 00 00 00 11		0
			{ 0x07 },	// 00 00 01 11		1
			{ 0x13 },	// 00 01 00 11		2
			{ 0x17 },	// 00 01 01 11		3
			{ 0x83 },	// 01 00 00 11		4
			{ 0x87 },	// 01 00 01 11		5
			{ 0x53 },	// 01 01 00 11		6
			{ 0xa7 },	// 10 10 10 11		7
			{ 0x57 },	// 01 01 01 11		8
			{ 0x0f },	// 00 00 11 11		9
			{ 0x33 },	// 00 11 00 11		a
			{ 0x3f },	// 00 11 11 11		b
			{ 0xc3 },	// 11 00 00 11		c
			{ 0xcf },	// 11 00 11 11		d
			{ 0xf3 },	// 11 11 00 11		e
			{ 0xff },	// 11 11 11 11		f
		};
		for (int t=0; t<16; t++)
		{
			bus->write(GFX_PAL_INDX, t);
			bus->write(GFX_PAL_DATA, ref[t].color);
			//printf("ref: $%02X, R:%1d, G:%1d, B:%1d, A:%1d\n", ref[t].color, ref[t].r, ref[t].g, ref[t].b, ref[t].a);
		}
	}

	OnCreate();

	// pre-initialize memories
	bus->debug_write_word(TIMING_WIDTH, _pix_width);
	bus->debug_write_word(TIMING_HEIGHT, _pix_height);
	
	// OnInitialize() all of the graphics mode layers
	for (int t = 0; t < 8; t++)
		m_gmodes[t]->OnInitialize();
	gfx_debug->OnInitialize();
	gfx_mouse->OnInitialize();
}

void GFX::OnQuit()
{
	//printf("GFX::OnQuit()\n");

	// destroy the palette
	palette.clear();
	
	// OnQuit() all of the graphics mode layers
	for (int t = 0; t < 8; t++)
		m_gmodes[t]->OnQuit();
	gfx_debug->OnQuit();
	gfx_mouse->OnQuit();
}

void GFX::OnEvent(SDL_Event *evnt) 
{
	if (evnt->type == SDL_KEYDOWN)
	{
		//// TEMPORARY: TESTING (Toggle the backbuffer)
		//if (evnt->key.keysym.sym == SDLK_SPACE)
		//{
		//	// flip the back buffer
		//	if (m_enable_backbuffer)
		//	{
		//		// m_current_backbuffer = 1 - m_current_backbuffer;
		//		
		//		//printf("GFX::OnEvent() --- current backbuffer: %d\n", m_current_backbuffer);
		//		Byte data = bus->read(GFX_FLAGS);
		//		data ^= 0x08;
		//		bus->write(GFX_FLAGS, data);
		//		//printf("GFX::OnEvent() --- current backbuffer: %d\n", m_current_backbuffer);
		//	}
		//}

		// toggle fullscreen/windowed
		if (evnt->key.keysym.sym == SDLK_RETURN)
		{
			if (SDL_GetModState() & KMOD_ALT)
			{
				Byte data = bus->read(GFX_AUX);			// this one does
				data ^= 0x80;
				bus->write(GFX_AUX, data);
				printf("FULLSCREEN TOGGLE\n");
			}
		}
		// change active display (monitor)
		SDL_Keymod km = SDL_GetModState();
		int num_displays = SDL_GetNumVideoDisplays() - 1;
		//if (km & KMOD_ALT)// && km & KMOD_CTRL)
		{
			//// toggle backbuffer enable
			//if (evnt->key.keysym.sym == SDLK_c)
			//{
			//	Byte data = bus->read(GFX_FLAGS);
			//	data ^= 0x40;
			//	bus->write(GFX_FLAGS, data);
			//}
			// toggle debug enable
			if (evnt->key.keysym.sym == SDLK_d)
			{
				Byte data = bus->read(GFX_FLAGS);
				data ^= 0x20;
				bus->write(GFX_FLAGS, data);
			}
			// left 
			if (evnt->key.keysym.sym == SDLK_LEFT)
			{
				Byte data = bus->read(GFX_AUX);
				Byte monitor = (data & 0x07);
				//printf("GFX::OnEvent() ---  monitor: %d\n", monitor);
				if (monitor > 0) 
				{
					monitor--;
					data &= 0xf8;
					data |= monitor;
					bus->write(GFX_AUX, data);
				}
			}
			// right
			if (evnt->key.keysym.sym == SDLK_RIGHT)
			{
				Byte data = bus->read(GFX_AUX);
				Byte monitor = (data & 0x07);
				if (monitor < num_displays)
				{
					monitor++;
					data &= 0xf8;
					data |= monitor;
					bus->write(GFX_AUX, data);
				}
			}
			// up (graphics mode index)
			if (evnt->key.keysym.sym == SDLK_UP)
			{
				m_gmode_index = bus->read(GFX_FLAGS) & 0x07;
				if (m_gmode_index<7) 
					m_gmode_index++;
				Byte data = (bus->read(GFX_FLAGS) & 0xf8) | m_gmode_index;
				bus->write(GFX_FLAGS, data);
			}
			// down (graphics mode index)
			if (evnt->key.keysym.sym == SDLK_DOWN)
			{
				m_gmode_index = bus->read(GFX_FLAGS) & 0x07;
				if (m_gmode_index > 0)
					m_gmode_index--;
				Byte data = (bus->read(GFX_FLAGS) & 0xf8) | m_gmode_index;
				bus->write(GFX_FLAGS, data);
			}

			// [V] VSYNC toggle (GFX_FLAGS bit 6)
			if (evnt->key.keysym.sym == SDLK_v)
			{
				Byte data = bus->read(GFX_FLAGS);
				data ^= 0x80;
				bus->write(GFX_FLAGS, data);
			}
		}
	}
	m_gmodes[m_gmode_index]->OnEvent(evnt);
	if (DebugEnabled())
		gfx_debug->OnEvent(evnt);
	if (MouseEnabled())
		gfx_mouse->OnEvent(evnt);
}

void GFX::OnCreate() 
{
	//_window_width = 1366;
	//_window_height = int(float((_window_width) + 0.5f) / _aspect);

	// detect the desktop display size
	int display_max = SDL_GetNumVideoDisplays();
	if (m_display_num + 1 > display_max)
		m_display_num = display_max - 1;
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(m_display_num, &dm);

	int height = dm.h;	// 900;
	// int width = (int)float(height * m_vGres[m_video_res].aspect);
	int width = (int)float(height * _aspect);
	if (!m_fullscreen)
	{
		height -= 150; // 54;	// chop some vertical to account for the windowed title bar
		width = (int)float(height * _aspect);
	}

	// adjust the window size based on odd aspect ratio monitors
	if (width >= dm.w)
	{
		width = dm.w;
		// height = width / m_vGres[m_video_res].aspect;
		height = int((float)width / _aspect);
	}

	// create the window
	if (_window == nullptr)
	{
		_window = SDL_CreateWindow("X9_Retro6809",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, _window_flags);
		if (_window == nullptr)
		{
			std::string er = "Window could not be created! SDL_Error: %s";
			er += SDL_GetError();
			Bus::Err(er.c_str());
		}
		_surface = SDL_GetWindowSurface(_window);
	}

	// center the window in the appropriate display monitor
	//int dsply_num = (bus->read(GFX_DISPLAY_AUX) & GRES_AUX::GRES_AUX_DSPLY_MASK) % dsply_max;
	SDL_SetWindowPosition(_window,
		SDL_WINDOWPOS_CENTERED_DISPLAY(m_display_num), SDL_WINDOWPOS_CENTERED_DISPLAY(m_display_num));

	// set window to fullscreen?
	if (m_fullscreen)
	{
		if (SDL_SetWindowFullscreen(_window, _fullscreen_flags) < 0)
		{
			std::string err = "Failed to set fullscreen: \n";
			err += SDL_GetError();
			Bus::Err(err.c_str());
		}
	}

	// create the main renderer for the window
	if (_renderer == nullptr)
	{
		_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if (m_VSYNC)
			_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC;

		_renderer = SDL_CreateRenderer(_window, -1, _renderer_flags);
		if (_renderer == nullptr)
		{
			std::string er = "Renderer could not be created! SDL Error: %s";
			er += SDL_GetError();
			Bus::Err(er.c_str());
		}
		SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	}

	// create the main screen texture (with backbuffer)
	if (_texture[0] == nullptr)
		_texture[0] = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, _pix_width, _pix_height);
	if (_texture[1] == nullptr)
		_texture[1] = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, _pix_width, _pix_height);

	bIsDirty = false;
	//bWasInit = true;
	//bus->cpu_pause = false;

	// OnCreate all of the graphics mode layers
	for (int t = 0; t < 8; t++)
		m_gmodes[t]->OnCreate();
	gfx_debug->OnCreate();
	gfx_mouse->OnCreate();

	// output debug info to console
	const bool OUTPUT_ONCREATE = true;
	if (OUTPUT_ONCREATE)
	{
		std::string szMon[] = { "Middle", "Left", "Right" };
		printf("\n\n\n\n");
		printf("GFX::OnCreate(): \n");
		printf("         Timing: %d X %d\n", bus->read_word(TIMING_WIDTH), bus->read_word(TIMING_HEIGHT));
		printf("          VSYNC: %s\n", (bus->read(GFX_FLAGS) & 0x40) ? "true" : "false");
		printf(" Gfx Mode Index: %d\n", (bus->read(GFX_FLAGS) & 0x07) );
		printf("         Aspect: %f\n", _aspect);
		printf("        Monitor: %d\n", (bus->read(GFX_AUX) & 0x07) );
		printf("    Screen Mode: %s\n", (bus->read(GFX_AUX) & 0x80) ? "FULLSCREEN" : "WINDOWED");
		printf("    Back Buffer: %s\n", (bus->read(GFX_FLAGS) & 0x08) ? "1" : "0");
	}

	SDL_ShowCursor(SDL_DISABLE);
}

void GFX::OnDestroy()
{
	//bus->cpu_pause = true;

	// destroy all of the gnodes
	for (int t=0; t<8; t++)
		m_gmodes[t]->OnDestroy();
	gfx_debug->OnDestroy();
	gfx_mouse->OnDestroy();

	for (int t=0; t<2; t++)
	{
		if (_texture[t])
		{
			SDL_DestroyTexture(_texture[t]);
			_texture[t] = nullptr;
		}
	}
	if (_renderer)
	{
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}
	Uint32 flags = SDL_GetWindowFlags(_window);
	if (_surface && (flags & ~SDL_WINDOW_OPENGL)) // seg faults when SDL_WINDOW_OPENGL is NOT used to create the window
	{
		SDL_FreeSurface(_surface);
		_surface = nullptr;
	}
	if (_window)
	{
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}

	//bWasInit = false;
}

void GFX::OnUpdate(float fElapsedTime)
{
	//// automatically flip the backbuffer
	//Byte data = bus->read(GFX_FLAGS);
	//data ^= 0x08;
	//bus->write(GFX_FLAGS, data);

	// clear the screen
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0xFF);
	SDL_SetRenderTarget(_renderer, _texture[m_current_backbuffer]);

	// render the graphics mode
	m_gmodes[m_gmode_index]->OnUpdate(fElapsedTime);
	if (DebugEnabled())
		gfx_debug->OnUpdate(fElapsedTime);
	if (MouseEnabled())
		gfx_mouse->OnUpdate(fElapsedTime);

	// update the fps every second. The SDL_SetWindowTitle seems very slow
	// in Linux. Only call it once per second.
	const float cDelay = 1.0f;
	static float acc = fElapsedTime;
	acc += fElapsedTime;
	if (acc > fElapsedTime + cDelay)
	{
		acc -= cDelay;
		std::string title = "FPS: " + std::to_string(Bus::getFPS());
		SDL_SetWindowTitle(_window, title.c_str());
	}

	// render the GFX object to the main screen texture
	_onRender();
}

void GFX::_onRender()
{
	SDL_SetRenderTarget(_renderer, NULL);

	Uint32 window_flags = SDL_GetWindowFlags(_window);
	if (window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP)		// m_fullscreen
	{
		// fetch the actual current display resolution
		int ww, wh;
		SDL_GetWindowSize(_window, &ww, &wh);
		float fh = (float)wh;
		float fw = fh * _aspect;
		if (fw > ww)
		{
			fw = (float)ww;
			fh = fw / _aspect;
		}
		SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
		if (m_enable_backbuffer)
			SDL_RenderCopy(_renderer, _texture[1 - m_current_backbuffer], NULL, &dest);
		else
			SDL_RenderCopy(_renderer, _texture[m_current_backbuffer], NULL, &dest);
	}
	else
	{
		if (m_enable_backbuffer)
			SDL_RenderCopy(_renderer, _texture[1 - m_current_backbuffer], NULL, NULL);
		else
			SDL_RenderCopy(_renderer, _texture[m_current_backbuffer], NULL, NULL);
	}

	// render outputs
	m_gmodes[m_gmode_index]->OnRender();
	if (DebugEnabled())
		gfx_debug->OnRender();
	if (MouseEnabled())
		gfx_mouse->OnRender();

	// finally present the GFX chain 
	SDL_RenderPresent(_renderer);
}


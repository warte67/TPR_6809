// * GFX.cpp ***************************************
// *
// *   Acts to emulate a Raspberry PI Pico
// *		running an R4G4B4 VGA resistor ladder DAC
// *		circuit for graphic display.
// ************************************

#include "Bus.h"
#include "Memory.h"
#include "Device.h"
#include "GFX.h"


bool GFX::m_VSYNC       = false;	// true:VSYNC, false:not throttled
bool GFX::m_fullscreen  = false;	// true:fullscreen, false:windowed
int  GFX::m_display_num = 1;		// which monitor to use, default.


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
			{
				//      bit 7: fullscreen
				//      bit 6: vsync
				//      bit 5: unassigned
				//      bit 4: unassigned
				//      bit 3: unassigned
				//      bit 0-2: display monitor (0-7)
				Byte ret = 0;
				if (ptrGfx->m_fullscreen)	ret |= 0x80;
				if (ptrGfx->m_VSYNC)		ret |= 0x40;
				Byte num = ptrGfx->m_display_num & 0x07;
				ret |= num;

				Byte test = ptrGfx->debug_read(ofs); // TESTING!!!

				ptrGfx->debug_write(ofs, ret);	// pre-write			(NOT WORKING?)

				test = ptrGfx->debug_read(ofs); // TESTING!!!

				return ret;
			}

			// All we care about here is the resolution width/height. This represents the
			//     screen timing resolution the PICO will have to display.
			if (ofs == TIMING_WIDTH)
				return (ptrGfx->_res_width >> 8) & 0x00ff;
			if (ofs == TIMING_WIDTH + 1)
				return ptrGfx->_res_width & 0x00ff;
			if (ofs == TIMING_HEIGHT)
				return (ptrGfx->_res_height >> 8) & 0x00ff;
			if (ofs == TIMING_HEIGHT + 1)
				return ptrGfx->_res_height & 0x00ff;
		}
		else
		{	// WRITTEN TO

			if (ofs == GFX_FLAGS)
			{
				//      bit 7: fullscreen
				//      bit 6: vsync
				//      bit 5: unassigned
				//      bit 4: unassigned
				//      bit 3: unassigned
				//      bit 0-2: display monitor (0-7)
				//data = ptrGfx->debug_read(ofs);					(NOT WORKING?)
				//data=ptrGfx->bus->debug_read(ofs);
				ptrGfx->m_fullscreen = ((data & 0x80) == 0x80);
				ptrGfx->m_VSYNC = ((data & 0x40) == 0x40);
				ptrGfx->m_display_num = data & 0x07;
				int num = SDL_GetNumVideoDisplays();
				ptrGfx->m_display_num %= num;
				ptrGfx->bIsDirty = true;

				//ptrGfx->bus->debug_write(ofs, data);
				ptrGfx->debug_write(ofs, data);			//			(NOT WORKING?)
			}

			//if (ofs >= SCR_WIDTH && ofs <= PIX_HEIGHT + 1)
			//	return data;	// read only
		}
	}
	return data;
}



GFX::GFX() : REG(0,0)
{
	Device::_deviceName = "GFX";
	bus = Bus::getInstance();
	memory = bus->getMemoryPtr();
}
GFX::GFX(Word offset, Word size) : REG(offset, size)
{
	Device::_deviceName = "GFX";
	bus = Bus::getInstance();
	memory = bus->getMemoryPtr();
}
GFX::~GFX()
{    
}


Word GFX::MapDevice(MemoryMap* memmap, Word offset)
{
	std::string reg_name = "GFX System";
	DWord st_offset = offset;
	// Defined only to serve as a template for inherited device objects.
	// (this will never be called due to being an abstract base type.)
	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Graphics Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "GFX_FLAGS", "(Byte) gfx system flags:" }); offset += 1;
	memmap->push({ offset, "", "    bit 7: fullscreen" }); offset += 0;
	memmap->push({ offset, "", "    bit 6: vsync" }); offset += 0;
	memmap->push({ offset, "", "    bit 5: unassigned" }); offset += 0;
	memmap->push({ offset, "", "    bit 4: unassigned" }); offset += 0;
	memmap->push({ offset, "", "    bit 3: unassigned" }); offset += 0;
	memmap->push({ offset, "", "    bit 0-2: display monitor (0-7)" }); offset += 0;
	memmap->push({ offset, "TIMING_WIDTH", "(Word) timing width" }); offset += 2;
	memmap->push({ offset, "TIMING_HEIGHT", "(Word) timing height" }); offset += 2;

	return offset - st_offset;
}

void GFX::OnInitialize() {}

void GFX::OnEvent(SDL_Event *evnt) 
{
	if (evnt->type == SDL_KEYDOWN)
	{
		// toggle fullscreen/windowed
		if (evnt->key.keysym.sym == SDLK_RETURN)
		{
			if (SDL_GetModState() & KMOD_ALT)
			{
				// PROBLEM:  this->read(GFX_FLAGS) still returns 0xCC!
				//    and this->write(GFX_FLAGS) doesn't write correctly!
				//    MemBlocks still not attached to the GFX Object

				//Byte data = this->read(GFX_FLAGS);		// still doesn't work correctly
				Byte data = bus->read(GFX_FLAGS);			// this one does
				data ^= 0x80;
				bus->write(GFX_FLAGS, data);
				printf("FULLSCREEN TOGGLE\n");
			}
		}
		// change active display (monitor)
		SDL_Keymod km = SDL_GetModState();
		int num_displays = SDL_GetNumVideoDisplays() - 1;
		if (km & KMOD_ALT)// && km & KMOD_CTRL)
		{
			// left 
			if (evnt->key.keysym.sym == SDLK_LEFT)
			{
				Byte data = bus->read(GFX_FLAGS);
				Byte monitor = (data & 0x07);
				//printf("GFX::OnEvent() ---  monitor: %d\n", monitor);
				if (monitor > 0) 
				{
					monitor--;
					data &= 0xf8;
					data |= monitor;
					bus->write(GFX_FLAGS, data);
				}
			}
			// right
			if (evnt->key.keysym.sym == SDLK_RIGHT)
			{
				Byte data = bus->read(GFX_FLAGS);
				Byte monitor = (data & 0x07);
				if (monitor < num_displays)
				{
					monitor++;
					data &= 0xf8;
					data |= monitor;
					bus->write(GFX_FLAGS, data);
				}
			}
			// [V] VSYNC toggle (GFX_FLAGS bit 6)
			if (evnt->key.keysym.sym == SDLK_v)
			{
				Byte data = bus->read(GFX_FLAGS);
				data ^= 0x40;
				bus->write(GFX_FLAGS, data);
			}
		}
	}
}

void GFX::OnCreate() 
{
	//_window_width = 1366;
	//_window_height = int(float((_window_width) + 0.5f) / _aspect);

	// detect the desktop display size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(m_display_num, &dm);

	int height = dm.h;	// 900;
	// int width = (int)float(height * m_vGres[m_video_res].aspect);
	int width = (int)float(height * _aspect);
	if (!m_fullscreen)
	{
		height -= 54;	// chop some vertical to account for the windowed title bar
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
	_window = SDL_CreateWindow("X9_Retro6809",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, _window_flags);
	if (_window == nullptr)
	{
		std::string er = "Window could not be created! SDL_Error: %s";
		er += SDL_GetError();
		Bus::Err(er.c_str());
	}
	_surface = SDL_GetWindowSurface(_window);

	// center the window in the appropriate display monitor
	int dsply_max = SDL_GetNumVideoDisplays();
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

	// create the main screen texture	
	_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA4444,
		SDL_TEXTUREACCESS_TARGET, _res_width, _res_height);

	bIsDirty = false;
	//bWasInit = true;
	//bus->cpu_pause = false;

	// OnCreate all of the graphics mode layers
	//for (int t = 0; t < 8; t++)
	//	m_gmodes[t]->OnCreate((Word)width, (Word)height, m_vGres[m_video_res].res_width, m_vGres[m_video_res].res_height, aspect);

	// output debug info to console
	const bool OUTPUT_ONCREATE = true;
	if (OUTPUT_ONCREATE)
	{
		std::string szMon[] = { "Middle", "Left", "Right" };
		printf("\n\n\n\n");
		printf("GFX::OnCreate(): \n");
		printf("         Timing: %d X %d\n", bus->read_word(TIMING_WIDTH), bus->read_word(TIMING_HEIGHT));
		printf("          VSYNC: %s\n", (bus->read(GFX_FLAGS) & 0x40) ? "true" : "false");
		printf("         Aspect: %f\n", _aspect);
		printf("        Monitor: %d\n", bus->read(GFX_FLAGS) & 0x07);
		printf("    Screen Mode: %s\n", (bus->read(GFX_FLAGS) & 0x80) ? "FULLSCREEN" : "WINDOWED");
	}

	//SDL_ShowCursor(SDL_DISABLE);

}

void GFX::OnDestroy()
{
	//bus->cpu_pause = true;

	if (_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
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
	// clear the screen
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0xFF);
	SDL_SetRenderTarget(_renderer, _texture);
	// fill with a few random pixels
	const bool FILL_RAND_PIXELS = true;
	if (FILL_RAND_PIXELS)
	{
		for (int t = 0; t < 500; t++)
		{
			SDL_Rect dot = { rand() % _res_width,
					rand() % _res_height, 1, 1 };
			SDL_SetRenderDrawColor(_renderer, rand() % 256, rand() % 256, rand() % 256, 0xFF);
			SDL_RenderFillRect(_renderer, &dot);
		}
	}
	else
		SDL_RenderClear(_renderer);

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

	// render all of the sub-GFX "GfxNode" objects with Gfx only OnRender() 
	// virtual methods	to the main target texture
	// ...

	// finally present the GFX chain 
	SDL_RenderPresent(_renderer);
}

void GFX::_onRender()
{
	SDL_SetRenderTarget(_renderer, NULL);

	Uint32 window_flags = SDL_GetWindowFlags(_window);
	if (window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
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
		// printf("FULLSCREEN:\n");
		// printf("	aspect: %f\n", _aspect);
		// printf("	width:  %f\n", fw);
		// printf("	height: %f\n", fh);	
		
		SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
		SDL_RenderCopy(_renderer, _texture, NULL, &dest);
	}
	else
		SDL_RenderCopy(_renderer, _texture, NULL, NULL);
}


// void GFX::OnRender() 
// {
// 	// output this objects texture
// 	SDL_SetRenderTarget(_renderer, NULL);
// 	SDL_RenderCopy(_renderer, _texture, NULL, NULL);

// 	// only present from the GFX object
// 	SDL_RenderPresent(_renderer);
// }

void GFX::OnQuit() {}

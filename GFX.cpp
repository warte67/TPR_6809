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

GFX::GFX()
{
	Device::_deviceName = "GFX";
}
GFX::~GFX()
{    
}

Word GFX::MapDevice(MemoryMap* memmap, Word offset)
{
	DWord st_offset = offset;
    // Defined only to serve as a template for inherited device objects.
    // (this will never be called due to being an abstract base type.)
    memmap->push({ offset, "", "" }); offset += 0;
    memmap->push({ offset, "", "Graphics Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "BASE_GFX_REG",	"Base GFX Hardware Register" }); offset += 2;
	memmap->push({ offset, "GFX_REG2",		"GFX Register Number Two" }); offset += 2;
	memmap->push({ offset, "GFX_REG3",		"GFX Register Number Three" }); offset += 2;

	offset += bus->m_memory->AssignREG("GFX System", offset - st_offset, nullptr);

    return offset;
}


void GFX::OnInitialize() {}

void GFX::OnEvent(SDL_Event *evnt) {}

void GFX::OnCreate() 
{
	_window_width = 1366;
	_window_height = int(float(_window_width) / _aspect);

	Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
	_window = SDL_CreateWindow("SDL Window",
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					_window_width, _window_height,
					window_flags);
	if (_window == NULL)
	{
		std::string er = "Failed to create the window: ";
		er += SDL_GetError();
		Bus::Err(er.c_str());
	}
	_surface = SDL_GetWindowSurface(_window);
	if (_surface == NULL)
	{
		std::string er = "Failed to retrieve the windows surface: ";
		er += SDL_GetError();
		Bus::Err(er.c_str());
	}
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;	// | SDL_RENDERER_PRESENTVSYNC;
	_renderer = SDL_CreateRenderer(_window, -1, flags);
	if (_renderer == NULL)
	{
		std::string er = "Failed to create the renderer: ";
		er += SDL_GetError();
		Bus::Err(er.c_str());
	}
	_texture = SDL_CreateTexture(_renderer, 
							SDL_PIXELFORMAT_RGBA4444, 
							SDL_TEXTUREACCESS_TARGET, 
							_res_width, _res_height);
	if (_texture == NULL)
	{
		std::string er = "Failed to create the texture: ";
		er += SDL_GetError();
		Bus::Err(er.c_str());
	}
}

void GFX::OnDestroy()
{
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
	// update the fps every second
	const float cDelay = 1.0f;
	static float acc = fElapsedTime;
	acc += fElapsedTime;
	if (acc > fElapsedTime + cDelay)
	{
		acc -= cDelay;
		std::string title = "FPS: " + std::to_string(Bus::getFPS());
		SDL_SetWindowTitle(_window, title.c_str());}
	}

void GFX::OnRender() 
{
	// output this objects texture
	SDL_SetRenderTarget(_renderer, NULL);
	SDL_RenderCopy(_renderer, _texture, NULL, NULL);

	// only present from the GFX object
	SDL_RenderPresent(_renderer);
}

void GFX::OnQuit() {}

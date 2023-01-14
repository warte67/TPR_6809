// * GfxRaw.cpp ***************************************
// *
// * 128x80 x 4096-Color (16 bpp 20KB) - Serial Buffer / FPGA
// ************************************


#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxRaw.h"

// statics:
const int GfxRaw::pixel_width = 128;
const int GfxRaw::pixel_height = 80;


// constructor
GfxRaw::GfxRaw() : GfxMode()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

// destructor
GfxRaw::~GfxRaw()
{
}

void GfxRaw::OnInitialize()
{
}

void GfxRaw::OnActivate()
{
}
void GfxRaw::OnDeactivate()
{
}


void GfxRaw::OnCreate()
{
	if (bitmap_texture == nullptr)
	{
		bitmap_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pixel_width, pixel_height);
		SDL_SetTextureBlendMode(bitmap_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);
	}
}

void GfxRaw::OnDestroy()
{
	if (bitmap_texture)
	{
		SDL_DestroyTexture(bitmap_texture);
		bitmap_texture = nullptr;
	}
}

void GfxRaw::OnUpdate(float fElapsedTime)
{
	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);

		Word ofs = VIDEO_START;
		for (int y = 0; y < pixel_height; y++)
		{
			for (int x = 0; x < pixel_width; x++)
			{
				// Byte data = bus->debug_read(ofs++);

				// RRRR GGGG BBBB AAAA
				Word data = ((rand() % 4096) << 4) | 0xF;
				Byte r = Byte((data >> 12) & 0x0f); r += r << 4;
				Byte g = Byte((data >> 8) & 0x0f); g += g << 4;
				Byte b = Byte((data >> 4) & 0x0f); b += b << 4;
				Byte a = Byte((data >> 0) & 0x0f); a += a << 4;

				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);
			}
		}
	}
}

void GfxRaw::OnRender()
{
	SDL_SetRenderTarget(gfx->Renderer(), NULL);
	if (gfx->Fullscreen())
	{
		int ww, wh;
		SDL_GetWindowSize(gfx->Window(), &ww, &wh);
		float fh = (float)wh;
		float fw = fh * gfx->Aspect();
		if (fw > ww)
		{
			fw = (float)ww;
			fh = fw / gfx->Aspect();
		}
		SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
		SDL_RenderCopy(gfx->Renderer(), bitmap_texture, NULL, &dest);
	}
	else
		SDL_RenderCopy(gfx->Renderer(), bitmap_texture, NULL, NULL);
}


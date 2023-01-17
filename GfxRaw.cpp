// * GfxRaw.cpp ***************************************
// *
// * 256x160 x 64-Colors
// *		Requires a 40KB Buffer
// ************************************


#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxRaw.h"

// statics:
const int GfxRaw::pixel_width = 256;
const int GfxRaw::pixel_height = 160;

// Graphics Mode Unique Callback Function:
Byte GfxRaw::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		//printf("GfxRaw::OnCallback() -- READ\n");
	}
	else
	{	// WRITE
		//printf("GfxRaw::OnCallback() -- WRITE\n");
	}
	return data;
}

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
	//bus->write_word(GFX_EXT_ADDR, 0);
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
				Word data = bus->debug_read_word(ofs++);	// = 2);
				if (ofs > VIDEO_END)
					ofs = VIDEO_START;

				//Byte r = (data & 0xF000) >> 12;	r |= r << 4;
				//Byte g = (data & 0x0F00) >> 8;	g |= g << 4;
				//Byte b = (data & 0x00F0) >> 4;	b |= b << 4;
				//Byte a = (data & 0x000F) >> 0;	a |= a << 4;

				Byte r = (data & 0xC0) >> 6;	r |= r << 2 | r << 4 | r << 6;
				Byte g = (data & 0x30) >> 4;	g |= g << 2 | g << 4 | g << 6;
				Byte b = (data & 0x0C) >> 2;	b |= b << 2 | b << 4 | b << 6;
				Byte a = (data & 0x03) >> 0;	a |= a << 2 | a << 4 | a << 6;

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


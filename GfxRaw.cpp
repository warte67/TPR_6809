/**** GfxRaw.cpp ***************************************
 *
 * 128x80 x 4096-Color (16 bpp 20KB) - Serial Buffer / FPGA
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxRaw.h"


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


		Byte d = 0;
		for (int t = 0; t < 0x5000; t++)
		{
			bus->write_word(GFX_EXT_ADDR, t);
			if (d == 0xff)	d = 0;
			bus->write(GFX_EXT_DATA, d++);
		}
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

		// TESTING: ********************************
			//for (int t = 0; t < 0x5000; t++)
			//{
			//	bus->write_word(GFX_EXT_ADDR, t);
			//	Byte data = bus->read(GFX_EXT_DATA);
			//	data++;
			//	bus->write(GFX_EXT_DATA, data);
			//}
		// TESTING: ********************************


		Word addr = 0;
		for (int y = 0; y < pixel_height; y++)
		{
			for (int x = 0; x < pixel_width; x++)
			{
				Byte data = GfxMode::s_mem_64k[addr++];				
				Byte r = (data & 0xF0) >> 4;	r |= r << 4;
				Byte g = (data & 0x0F) >> 0;	g |= g << 4;
				data = GfxMode::s_mem_64k[addr++];
				Byte b = (data & 0xF0) >> 4;	b |= b << 4;
				Byte a = (data & 0x0F) >> 0;	a |= a << 4;
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


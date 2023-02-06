/**** GfxBmp16.cpp ***************************************
 *
 *  128x80 x 16-Color BMP Graphics Mode
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxBmp16.h"


// Graphics Mode Unique Callback Function:
Byte GfxBmp16::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		if (bWasRead)
		{	// READ	
			// GFX_FG_WDTH and GFX_FG_HGHT  (read only)
			if (ofs >= GFX_FG_WDTH && ofs <= GFX_FG_HGHT + 1)
			{
				Word fg_Width = pixel_width-1;
				Word fg_Height = pixel_height-1;
				if (ofs == GFX_FG_WDTH)		data = fg_Width;
				if (ofs == GFX_FG_HGHT)		data = fg_Height;
				bus->debug_write(ofs, data);
			}
		}
	}
	else
	{	// WRITE
	}
	return data;
}


// constructor
GfxBmp16::GfxBmp16() : GfxMode()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

// destructor
GfxBmp16::~GfxBmp16()
{
}

void GfxBmp16::OnInitialize()
{
}

void GfxBmp16::OnActivate()
{
	// load the default palette
	if (default_palette.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
			{ 0x0000 },	// 0000 0000.0000 1111		0
			{ 0x005F },	// 0000 0000.0101 1111		1
			{ 0x050F },	// 0000 0101.0000 1111		2
			{ 0x055F },	// 0000 0101.0101 1111		3
			{ 0x500F },	// 0101 0000.0000 1111		4
			{ 0x505F },	// 0101 0000.0101 1111		5
			{ 0x550F },	// 0101 0101.0000 1111		6
			{ 0xAAAF },	// 1010 1010.1010 1111		7
			{ 0x555F },	// 0101 0101.0101 1111		8
			{ 0x00FF },	// 0000 0000.1111 1111		9
			{ 0x0F0F },	// 0000 1111.0000 1111		a
			{ 0x0FFF },	// 0000 1111.1111 1111		b
			{ 0xF00F },	// 1111 0000.0000 1111		c
			{ 0xF0FF },	// 1111 0000.1111 1111		d
			{ 0xFF0F },	// 1111 1111.0000 1111		e
			{ 0xFFFF },	// 1111 1111.1111 1111		f
		};
		for (int t = 0; t < 16; t++)
			default_palette.push_back(ref[t]);
		// add blank entries for the rest of the 256 color palette entries
		GFX::PALETTE blank{ 0 };
		while (default_palette.size() < 256)
			default_palette.push_back(blank);
	}

	// load the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxBmp16::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, gfx->palette[t].color);
	}
}


void GfxBmp16::OnCreate() 
{
	if (bitmap_texture == nullptr)
	{
		bitmap_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pixel_width, pixel_height);
		SDL_SetTextureBlendMode(bitmap_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);
	}
}

void GfxBmp16::OnDestroy() 
{
	if (bitmap_texture)
	{
		SDL_DestroyTexture(bitmap_texture);
		bitmap_texture = nullptr;
	}
}

void GfxBmp16::OnUpdate(float fElapsedTime) 
{
	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);

		SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0);
		SDL_RenderClear(gfx->Renderer());

		Word ofs = VIDEO_START;
		for (int y = 0; y < pixel_height; y++)
		{
			for (int x = 0; x < pixel_width; x += 2)
			{
				Byte data = bus->debug_read(ofs++);
				Byte c1 = data >> 4;
				Byte c2 = data & 0x0f;
			
				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c1), gfx->grn(c1), gfx->blu(c1), gfx->alf(c1));
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);
				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c2), gfx->grn(c2), gfx->blu(c2), gfx->alf(c2));
				SDL_RenderDrawPoint(gfx->Renderer(), x + 1, y);

				//Byte a1 = int((float)gfx->alf(c1) * delta_alpha);
				//Byte a2 = int((float)gfx->alf(c2) * delta_alpha);
				//SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c1), gfx->grn(c1), gfx->blu(c1), a1);
				//SDL_RenderDrawPoint(gfx->Renderer(), x, y);
				//SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c2), gfx->grn(c2), gfx->blu(c2), a2);
				//SDL_RenderDrawPoint(gfx->Renderer(), x + 1, y);
			}
		}
	}
}

void GfxBmp16::OnRender() 
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

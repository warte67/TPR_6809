/**** GfxGlyph32.cpp ***************************************
 *
 *  32x20 Text Glyph Mode
 *        include FOUR layers of overlayed text
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#include "types.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxGlyph32.h"

//#include "font8x8_system.h"
extern Byte font8x8_system[256][8];


// Graphics Mode Unique Callback Function:
Byte GfxGlyph32::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		// GFX_FG_WDTH and GFX_FG_HGHT  (read only)
		if (ofs >= GFX_FG_WDTH && ofs <= GFX_FG_HGHT + 1)
		{
			Word fg_Width = 31;
			Word fg_Height = 19;
			if (ofs == GFX_FG_WDTH)		data = fg_Width;
			if (ofs == GFX_FG_HGHT)		data = fg_Height;
			bus->debug_write(ofs, data);
		}
		// read the font index
		if (ofs == GFX_FONT_IDX)
		{
			data = font_index;
			bus->debug_write(ofs, data);
		}
		// read character font data from the buffer
		if (ofs >= GFX_FONT_DAT && ofs <= GFX_FONT_DAT + 8)
		{
			data = glyph_data[ofs - GFX_FONT_DAT][font_index];
			bus->debug_write(ofs, data);
		}
	}
	else
	{	// WRITE
		// write the font index
		if (ofs == GFX_FONT_IDX)
		{
			font_index = data;
			bus->debug_write(ofs, data);
		}
		// write character font data to buffer and to texture
		if (ofs >= GFX_FONT_DAT && ofs <= GFX_FONT_DAT + 8)
		{
			glyph_data[ofs - GFX_FONT_DAT][font_index] = data;
			bus->debug_write(ofs, data);
			// write the new pixel data
			SDL_BlendMode old_blendmode;
			SDL_GetRenderDrawBlendMode(gfx->Renderer(), &old_blendmode);
			SDL_Texture* glyph = glyph_textures[font_index];
			SDL_SetRenderTarget(gfx->Renderer(), glyph);
			SDL_SetRenderDrawBlendMode(gfx->Renderer(), SDL_BLENDMODE_NONE);	//SDL_BLENDMODE_NONE
			int y = ofs - GFX_FONT_DAT;
			for (int x = 0; x < 8; x++)
			{
				Byte bitMask = 1 << (7 - x);
				if (data & bitMask)
					SDL_SetRenderDrawColor(gfx->Renderer(), 255, 255, 255, 255);
				else
					SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);
			}
			SDL_SetRenderDrawBlendMode(gfx->Renderer(), old_blendmode);
		}
	}
	return data;
}

GfxGlyph32::GfxGlyph32()
{
	//printf("GfxGlyph32::GfxGlyph32()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

void GfxGlyph32::OnInitialize()
{
	// set the initial character dimensions
	bus->debug_write(GFX_FG_WDTH, 31);
	bus->debug_write(GFX_FG_HGHT, 19);

	// setup initial font defaults
	for (int i = 0; i < 256; i++)
		for (int b = 0; b < 8; b++)
			glyph_data[i][b] = font8x8_system[i][b];

	//OnActivate();
}

void GfxGlyph32::OnActivate()
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
			{ 0x631F },	// 0101 0101.0000 1111		6		{ 0x550F }
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
void GfxGlyph32::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, gfx->palette[t].color);
	}
}

void GfxGlyph32::OnQuit()
{
	//printf("GfxGlyph::OnQuit()\n");
}

void GfxGlyph32::OnCreate()
{
	//printf("GfxGlyph32::OnCreate()\n");
	// 
	// create the glyph textures
	if (glyph_textures.size() == 0)
	{
		for (int t = 0; t < 256; t++)
		{
			SDL_Texture* glyph = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
				SDL_TEXTUREACCESS_TARGET, 8, 8);
			SDL_SetRenderTarget(gfx->Renderer(), glyph);
			SDL_SetTextureBlendMode(glyph, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0);
			SDL_RenderClear(gfx->Renderer());
			for (int y = 0; y < 8; y++)
			{
				for (int x = 0; x < 8; x++)
				{
					Byte bitMask = 1 << (7 - x);
					//if (font8x8_system[t][y] & bitMask)
					if (glyph_data[t][y] & bitMask)
					{
						SDL_SetRenderDrawColor(gfx->Renderer(), 255, 255, 255, 255);
						SDL_RenderDrawPoint(gfx->Renderer(), x, y);
					}
				}
			}
			glyph_textures.push_back(glyph);
		}
	}
	// create the main texture
	if (_glyph_texture == nullptr)
	{
		int pw = gfx->PixWidth() / 2;
		int ph = gfx->PixHeight() / 2;
		_glyph_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pw, ph);
		SDL_SetTextureBlendMode(_glyph_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);
	}
}
void GfxGlyph32::OnDestroy()
{
	//printf("GfxGlyph::OnDestroy()\n");

	// destroy the glyph textures
	for (auto& a : glyph_textures)
		SDL_DestroyTexture(a);
	glyph_textures.clear();

	// destroy the main texture
	if (_glyph_texture)
	{
		SDL_DestroyTexture(_glyph_texture);
		_glyph_texture = nullptr;
	}
}


void GfxGlyph32::OnUpdate(float fElapsedTime)
{
	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);

		SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0);
		SDL_RenderClear(gfx->Renderer());

		// display four layers of text
		for (int layer = VIDEO_START; layer < VIDEO_END; layer += 1280)
		{
			for (int ofs = layer; ofs < layer + 1280; ofs += 2)
			//for (int ofs = VIDEO_START; ofs < VIDEO_START + 1280; ofs += 2)	// <= VIDEO_END; ofs += 2)
			{
				// attribute bites:   FFFF BBBB
				Word index = ofs - VIDEO_START;
				Byte glyph = bus->read(ofs);
				Byte attr = bus->read(ofs + 1);
				int x = ((index / 2) % 32) * 8;
				int y = ((index / 2) / 32) * 8;
				// draw background
				Byte bg = attr & 0x0f;
				Byte red = gfx->red(bg);
				Byte grn = gfx->grn(bg);
				Byte blu = gfx->blu(bg);
				Byte alf = gfx->alf(bg);
				SDL_SetRenderDrawColor(gfx->Renderer(), red, grn, blu, alf);
				SDL_Rect dst = { x, y, 8, 8 };
				SDL_RenderFillRect(gfx->Renderer(), &dst);

				// draw foreground
				Byte fg = (attr >> 4) & 0x0f;
				red = gfx->red(fg);
				grn = gfx->grn(fg);
				blu = gfx->blu(fg);
				alf = gfx->alf(fg);
				int row = x / 8;
				int col = y / 8;

				OutGlyph(row, col, glyph, red, grn, blu, false);
			}
		}
	}
}

void GfxGlyph32::OutGlyph(int row, int col, Byte glyph, Byte red = 255, Byte grn = 255, Byte blu = 255, bool bDropShadow = false)
{
	SDL_Rect dst = { row * 8, col * 8, 8, 8 };
	SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);
	if (bDropShadow)
	{
		SDL_Rect drop = { dst.x + 1, dst.y + 1, dst.w, dst.h };
		SDL_SetTextureColorMod(glyph_textures[glyph], 0, 0, 0);
		SDL_RenderCopy(gfx->Renderer(), glyph_textures[glyph], NULL, &drop);
	}
	SDL_SetTextureColorMod(glyph_textures[glyph], red, grn, blu);
	SDL_RenderCopy(gfx->Renderer(), glyph_textures[glyph], NULL, &dst);
}

void GfxGlyph32::OnRender()
{
	//SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	//SDL_RenderCopy(gfx->Renderer(), _glyph_texture, NULL, NULL);

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
		SDL_RenderCopy(gfx->Renderer(), _glyph_texture, NULL, &dest);
	}
	else
		SDL_RenderCopy(gfx->Renderer(), _glyph_texture, NULL, NULL);
}


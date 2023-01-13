// * GfxGlyph64.cpp ***************************************
// *
// *  64x40 Text Glyph Mode 
// ************************************

#include "types.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxGlyph64.h"

//#include "font8x8_system.h"
extern Byte font8x8_system[256][8];


GfxGlyph64::GfxGlyph64()
{
	//printf("GfxGlyph64::GfxGlyph64()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
	if (gfx == nullptr)
	{
		Bus::Err("ERROR: GfxGlyph64::GfxGlyph64() -- gfx == nullptr");
	}
}

void GfxGlyph64::OnInitialize()
{
	 //printf("GfxGlyph::OnInitialize()\n");
	 // 
	// load the default palette
	if (default_palette.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
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
		for (int t = 0; t < 16; t++)
			default_palette.push_back(ref[t]);
	}
}

void GfxGlyph64::OnActivate()
{
	// load the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxGlyph64::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, gfx->palette[t].color);
	}
}

void GfxGlyph64::OnQuit()
{
	//printf("GfxGlyph::OnQuit()\n");
}

void GfxGlyph64::OnCreate()
{
	//printf("GfxGlyph::OnCreate()\n");
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
					if (font8x8_system[t][y] & bitMask)
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
		int pw = gfx->PixWidth();
		int ph = gfx->PixHeight();
		_glyph_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pw, ph);
		SDL_SetTextureBlendMode(_glyph_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);
	}
}
void GfxGlyph64::OnDestroy()
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


void GfxGlyph64::OnUpdate(float fElapsedTime)
{
	// printf("GfxGlyph::OnUpdate()\n");

	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);
		for (int ofs = VIDEO_START; ofs <= VIDEO_END; ofs += 2)
		{
			Word index = ofs - VIDEO_START;
			Byte glyph = bus->read(ofs);
			Byte attr = bus->read(ofs + 1);
			int x = ((index / 2) % 64) * 8;
			int y = ((index / 2) / 64) * 8;
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

			//if (fg==0)
			//	OutGlyph(row, col, glyph, red, grn, blu, false);
			//else
			//	OutGlyph(row, col, glyph, red, grn, blu, true);
		}
	}
}

void GfxGlyph64::OutGlyph(int row, int col, Byte glyph, Byte red=255, Byte grn=255, Byte blu=255, bool bDropShadow = false)
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

void GfxGlyph64::OnRender()
{
	// SDL_SetRenderTarget(gfx->Renderer(), NULL);
	SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_RenderCopy(gfx->Renderer(), _glyph_texture, NULL, NULL);
}


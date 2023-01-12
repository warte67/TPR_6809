// * GfxGlyph.cpp ***************************************
// *
// *  64x40 Text Glyph Mode 
// ************************************

#include "types.h"
#include "font8x8_system.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxGlyph.h"


GfxGlyph::GfxGlyph()
{
	//printf("GfxGlyph::GfxGlyph()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}



void GfxGlyph::OnInitialize()
{
	 printf("GfxGlyph::OnInitialize()\n");

	 if (pallette.size() == 0)
	 {
		 pallette.push_back({ 0x00, 0x00, 0x00,  SDL_ALPHA_OPAQUE });	// 0
		 pallette.push_back({ 0x00, 0x00, 0x55,  SDL_ALPHA_OPAQUE });	// 1
		 pallette.push_back({ 0x00, 0x55, 0x00,  SDL_ALPHA_OPAQUE });	// 2
		 pallette.push_back({ 0x00, 0x55, 0x55,  SDL_ALPHA_OPAQUE });	// 3
		 pallette.push_back({ 0x55, 0x00, 0x00,  SDL_ALPHA_OPAQUE });	// 4
		 pallette.push_back({ 0x55, 0x00, 0x55,  SDL_ALPHA_OPAQUE });	// 5
		 pallette.push_back({ 0x55, 0x55, 0x00,  SDL_ALPHA_OPAQUE });	// 6
		 pallette.push_back({ 0xaa, 0xaa, 0xaa,  SDL_ALPHA_OPAQUE });	// 7
		 pallette.push_back({ 0x55, 0x55, 0x55,  SDL_ALPHA_OPAQUE });	// 8
		 pallette.push_back({ 0x00, 0x00, 0xff,  SDL_ALPHA_OPAQUE });	// 9
		 pallette.push_back({ 0x00, 0xff, 0x00,  SDL_ALPHA_OPAQUE });	// a
		 pallette.push_back({ 0x00, 0xff, 0xff,  SDL_ALPHA_OPAQUE });	// b
		 pallette.push_back({ 0xff, 0x00, 0x00,  SDL_ALPHA_OPAQUE });	// c
		 pallette.push_back({ 0xff, 0x00, 0xff,  SDL_ALPHA_OPAQUE });	// d
		 pallette.push_back({ 0xff, 0xff, 0x00,  SDL_ALPHA_OPAQUE });	// e
		 pallette.push_back({ 0xff, 0xff, 0xff,  SDL_ALPHA_OPAQUE });	// f
	 }
}

void GfxGlyph::OnQuit()
{
	printf("GfxGlyph::OnQuit()\n");
}

void GfxGlyph::OnCreate()
{
	printf("GfxGlyph::OnCreate()\n");
	if (_glyph_texture == nullptr)
	{		
		_glyph_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, gfx->PixWidth(), gfx->PixHeight());
		SDL_SetTextureBlendMode(_glyph_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);

		//SDL_SetRenderDrawColor(gfx->Renderer(), 128, 32, 0, 0x80);
		//SDL_RenderClear(gfx->Renderer());
	}

	// TESTING: fill the first 256 bytes of screen ram with ascending values to display
	Byte ch = 0;
	Byte at = 0;
	Byte count = 0;
	for (int ofs = VIDEO_START; ofs <= VIDEO_END; ofs += 2)
	{
		bus->write(ofs, ch++);
		bus->write(ofs + 1, at);
		if (count++ > 8)
			at++;
	}
}
void GfxGlyph::OnDestroy()
{
	if (_glyph_texture)
	{
		SDL_DestroyTexture(_glyph_texture);
		_glyph_texture;
	}
}


void GfxGlyph::OnUpdate(float fElapsedTime)
{
	// printf("GfxGlyph::OnUpdate()\n");

	// only update once every 15ms
	const float delay = 0.015f;
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
			Byte red = pallette[bg].r;
			Byte grn = pallette[bg].g;
			Byte blu = pallette[bg].b;
			Byte alf = pallette[bg].a;
			SDL_SetRenderDrawColor(gfx->Renderer(), red, grn, blu, alf);
			SDL_Rect dst = { x, y, 8, 8 };
			SDL_RenderFillRect(gfx->Renderer(), &dst);

			// draw foreground
			Byte fg = (attr >> 4) & 0x0f;
			red = pallette[fg].r;
			grn = pallette[fg].g;
			blu = pallette[fg].b;
			alf = pallette[fg].a;
			SDL_SetRenderDrawColor(gfx->Renderer(), red, grn, blu, alf);
			for (int row = 0; row < 8; row++)
			{
				Byte data = font8x8_system[glyph][row];
				for (int col = 0; col < 8; col++)
				{
					//SDL_SetRenderDrawColor(gfx->Renderer(), 255, 255, 255, 0xFF);
					if (data & (1 << (7-col)))						
						SDL_RenderDrawPoint(gfx->Renderer(), x+col, y+row);
				}
			}
		}

	}
}

void GfxGlyph::OnRender()
{
	SDL_SetRenderTarget(gfx->Renderer(), NULL);
	SDL_RenderCopy(gfx->Renderer(), _glyph_texture, NULL, NULL);
}

void GfxGlyph::OnActivate()
{
	//printf("GfxGlyph::OnActivate()\n");

	// initially clear the screen
	SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0xFF);
	SDL_RenderClear(gfx->Renderer());
}
void GfxGlyph::OnDeactivate()
{
	//printf("GfxGlyph::OnDectivate()\n");
}

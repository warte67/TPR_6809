// * GfxGlyph.cpp ***************************************
// *
// *  64x40 Text Glyph Mode 
// ************************************

#include "types.h"
#include "font8x8_system.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxGlyph.h"

GfxGlyph::GfxGlyph()
{
	//printf("GfxGlyph::GfxGlyph()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
	if (gfx == nullptr)
	{
		Bus::Err("ERROR: GfxGlyph::GfxGlyph() -- gfx == nullptr");
	}
}

void GfxGlyph::OnInitialize()
{
	 printf("GfxGlyph::OnInitialize()\n");

	 if (_glyph_texture == nullptr)
	 {
		 int pw = gfx->PixWidth();
		 int ph = gfx->PixHeight();
		 _glyph_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			 SDL_TEXTUREACCESS_TARGET, pw, ph);
		 SDL_SetTextureBlendMode(_glyph_texture, SDL_BLENDMODE_BLEND);
		 SDL_SetRenderTarget(gfx->Renderer(), _glyph_texture);

		 //SDL_SetRenderDrawColor(gfx->Renderer(), 128, 32, 0, 0x80);
		 //SDL_RenderClear(gfx->Renderer());
	 }
}

void GfxGlyph::OnQuit()
{
	printf("GfxGlyph::OnQuit()\n");
	if (_glyph_texture)
	{
		SDL_DestroyTexture(_glyph_texture);
		_glyph_texture;
	}
}

void GfxGlyph::OnCreate()
{
	//printf("GfxGlyph::OnCreate()\n");

}
void GfxGlyph::OnDestroy()
{
	//printf("GfxGlyph::OnDestroy()\n");

}


void GfxGlyph::OnUpdate(float fElapsedTime)
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

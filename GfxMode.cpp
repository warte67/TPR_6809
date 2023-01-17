// * GfxMode.cpp ***************************************
// *
// *  Base graphics node.
// ************************************

#include <array>
#include "types.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"

std::array<Byte, 65536> GfxMode::s_mem_64k;
Word GfxMode::s_mem_64k_adr = 0;


GfxMode::GfxMode() 
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

GfxMode::~GfxMode() {}


void GfxMode::OnUpdate(float fElapsedTime)
{
	// MODE ZERO or null mode:
	// 
	// fill with a few random pixels noise
	for (int x = 0; x < gfx->PixWidth(); x++)
	{
		for (int y = 0; y < gfx->PixHeight(); y++)
		{
			Uint8 rnd = (rand() % 4) * 85;
			SDL_SetRenderDrawColor(gfx->Renderer(), rnd, rnd, rnd, 0xFF);
			SDL_RenderDrawPoint(gfx->Renderer(), x, y);
		}
	}
}


//// GfxNull ///////////////////////////////////////////////////



// Graphics Mode Unique Callback Function:
Byte GfxNull::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
	}
	else
	{	// WRITE
	}
	return data;
}

GfxNull::GfxNull() : GfxMode()
{
}


void GfxNull::OnUpdate(float fElapsedTime) 
{
	const float delay = 0.100f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		// MODE ZERO or null mode:
		const bool color_cycle = false;
		if (color_cycle)
		{
			static Uint8 gr = 0;
			static Uint8 ac = 0x55;
			static Uint8 co = 0;
			Uint8 r = 0;
			Uint8 g = 0;
			Uint8 b = 0;

			switch (co)
			{
				case 0: b = gr; break;
				case 1: g = gr; break;
				case 2: b = gr; g = gr; break;
				case 3: r = gr; break;
				case 4: r = gr; b = gr; break;
				case 5: r = gr; g = gr; break;
				case 6: r = gr; g = gr; b = gr; break;
			}

			SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, 0xFF);
			gr += ac;
			if (gr == 0xff || gr == 0x00)	ac *= -1;
			if (gr == 0) { co++; co %= 7; }
			SDL_RenderClear(gfx->Renderer());
		}
		else
		{
			Byte red = gfx->red(0);
			Byte grn = gfx->grn(0);
			Byte blu = gfx->blu(0);
			SDL_SetRenderDrawColor(gfx->Renderer(), red, grn, blu, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(gfx->Renderer());
		}
	}
}

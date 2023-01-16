// * GfxMode.cpp ***************************************
// *
// *  Base graphics node.
// ************************************

#include "types.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"


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

	//for (int t = 0; t < 500; t++)
	//{
	//	int x = rand() % gfx->PixWidth();
	//	int y = rand() % gfx->PixHeight();
	//	int r = rand() % 2;
	//	SDL_SetRenderDrawColor(gfx->Renderer(), r, r, r, 0xFF);
	//	SDL_RenderDrawPoint(gfx->Renderer(), x, y);
	//}
}


//// GfxNull ///////////////////////////////////////////////////

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
		//Uint8 r = (rand() % 4) * 85;
		//Uint8 g = (rand() % 4) * 85;
		//Uint8 b = (rand() % 4) * 85;
		SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, 0xFF);
		gr += ac;
		if (gr == 0xff || gr == 0x00)	ac *= -1;
		if (gr == 0) { co++; co %= 7; }
		SDL_RenderClear(gfx->Renderer());
	}
}

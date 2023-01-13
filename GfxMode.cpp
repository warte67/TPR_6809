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
}


//// GfxNull ///////////////////////////////////////////////////

GfxNull::GfxNull() : GfxMode()
{
}


void GfxNull::OnUpdate(float fElapsedTime) 
{
	// MODE ZERO or null mode:
	SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0xFF);
	SDL_RenderClear(gfx->Renderer());

	//// fill with a few random pixels noise
	//for (int x = 0; x < gfx->PixWidth(); x++)
	//{
	//	for (int y = 0; y < gfx->PixHeight(); y++)
	//	{
	//		Uint8 r = (rand() % 4) * 85;
	//		Uint8 g = (rand() % 4) * 85;
	//		Uint8 b = (rand() % 4) * 85;
	//		SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, 0xFF);
	//		SDL_RenderDrawPoint(gfx->Renderer(), x, y);
	//	}
	//}
}

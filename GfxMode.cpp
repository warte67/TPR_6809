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

	SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0xFF);
	SDL_RenderClear(gfx->Renderer());

	//// fill with a few random pixels noise
	//for (int t = 0; t < 1000; t++)
	//{
	//	SDL_Rect dot = { rand() % gfx->PixWidth(),
	//			rand() % gfx->PixHeight(), 1, 1 };
	//	Uint8 rnd = (rand() % 2) * 255;
	//	SDL_SetRenderDrawColor(gfx->Renderer(), rnd, rnd, rnd, 0xFF);
	//	SDL_RenderFillRect(gfx->Renderer(), &dot);
	//}
}

void GfxMode::OnActivate()
{
	//printf("GfxMode::OnActivate()\n");
}
void GfxMode::OnDeactivate()
{
	//printf("GfxMode::OnDectivate()\n");
}


//// GfxNull ///////////////////////////////////////////////////

GfxNull::GfxNull() : GfxMode()
{
}


void GfxNull::OnUpdate(float fElapsedTime) 
{
	// MODE ZERO or null mode:
	// fill with a few random pixels noise
	for (int t = 0; t < 1000; t++)
	{
		int x = rand() % gfx->PixWidth();
		int y = rand() % gfx->PixHeight();
		SDL_SetRenderDrawColor(gfx->Renderer(), rand() % 256, rand() % 256, rand() % 256, 0xFF);
		SDL_RenderDrawPoint(gfx->Renderer(), x, y);
	}
}

void GfxNull::OnActivate()
{
	//printf("GfxNull::OnActivate()\n");

	SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0xFF);
	SDL_RenderClear(gfx->Renderer());
}
void GfxNull::OnDeactivate()
{
	//printf("GfxNull::OnDectivate()\n");
}

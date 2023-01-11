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
		SDL_Rect dot = { rand() % gfx->PixWidth(),
				rand() % gfx->PixHeight(), 1, 1};
		SDL_SetRenderDrawColor(gfx->Renderer(), rand() % 256, rand() % 256, rand() % 256, 0xFF);
		SDL_RenderFillRect(gfx->Renderer(), &dot);
	}

}
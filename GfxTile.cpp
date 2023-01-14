// * GfxTile.h ***************************************
// *
// *  16x16 Tile Mode
// ************************************

#include "types.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxTile.h"



GfxTile::GfxTile()
{
	//printf("GfxTile::GfxTile()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
	if (gfx == nullptr)
	{
		Bus::Err("ERROR: GfxTile::GfxGlyph64() -- gfx == nullptr");
	}
}

void GfxTile::OnInitialize()
{
	//printf("GfxTile::OnInitialize()\n");
	// 
   // load the default palette
	if (default_palette.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
			{ 0x000F },	// 0000 0000 0000 1111		0
			{ 0x005F },	// 0000 0000 0101 1111		1
			{ 0x050F },	// 0000 0101 0000 1111		2
			{ 0x055F },	// 0000 0101 0101 1111		3
			{ 0x500F },	// 0101 0000 0000 1111		4
			{ 0x505F },	// 0101 0000 0101 1111		5
			{ 0x550F },	// 0101 0101 0000 1111		6
			{ 0xCCCF },	// 1010 1010 1010 1111		7
			{ 0x555F },	// 0101 0101 0101 1111		8
			{ 0x00FF },	// 0000 0000 1111 1111		9
			{ 0x0F0F },	// 0000 1111 0000 1111		a
			{ 0x0FFF },	// 0000 1111 1111 1111		b
			{ 0xF00F },	// 1111 0000 0000 1111		c
			{ 0xF0FF },	// 1111 0000 1111 1111		d
			{ 0xFF0F },	// 1111 1111 0000 1111		e
			{ 0xFFFF },	// 1111 1111 1111 1111		f
		};
		for (int t = 0; t < 16; t++)
			default_palette.push_back(ref[t]);
	}
}

void GfxTile::OnActivate()
{
	// load the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxTile::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, gfx->palette[t].color);
	}
}

void GfxTile::OnQuit()
{
	//printf("GfxGlyph::OnQuit()\n");
}

void GfxTile::OnCreate()
{
	//printf("GfxGlyph::OnCreate()\n");
	// 
	// create the tile textures
	// ...

	// create the main texture
	if (_tile_texture == nullptr)
	{
		int pw = gfx->PixWidth() / 2;
		int ph = gfx->PixHeight() / 2;
		_tile_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pw, ph);
		SDL_SetTextureBlendMode(_tile_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), _tile_texture);
	}
}
void GfxTile::OnDestroy()
{
	//printf("GfxGlyph::OnDestroy()\n");

	// destroy the tile textures
	// ...

	// destroy the main texture
	if (_tile_texture)
	{
		SDL_DestroyTexture(_tile_texture);
		_tile_texture = nullptr;
	}
}


void GfxTile::OnUpdate(float fElapsedTime)
{
	// printf("GfxGlyph::OnUpdate()\n");

	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), _tile_texture);

		// TEMPORARY: For now just display static
		for (int x = 0; x < gfx->PixWidth(); x++)
		{
			for (int y = 0; y < gfx->PixHeight(); y++)
			{
				Uint8 r = (rand() % 4) * 85;
				Uint8 g = (rand() % 4) * 85;
				Uint8 b = (rand() % 4) * 85;
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, 0xFF);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);
			}
		}
	}
}

void GfxTile::OnRender()
{
	// SDL_SetRenderTarget(gfx->Renderer(), NULL);
	SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_RenderCopy(gfx->Renderer(), _tile_texture, NULL, NULL);
}

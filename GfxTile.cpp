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

void GfxTile::OnActivate()
{
	// load the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxTile::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, gfx->palette[t].color);
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

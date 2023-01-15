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

// statics:
const int GfxTile::pixel_width = 512;		// or 256 for "Double Scan"
const int GfxTile::pixel_height = 320;		// or 160 for "Double Scan"

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
		int pw = pixel_width;
		int ph = pixel_height;
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
		Word ofs = VIDEO_START;
		for (int y = 0; y < pixel_height; y+=16)
		{
			for (int x = 0; x < pixel_width; x+=16)
			{
				Word data = bus->debug_read_word(ofs++);	// = 2);
				if (ofs > VIDEO_END)
					ofs = VIDEO_START;

				//Byte r = (data & 0xF000) >> 12;	r |= r << 4;
				//Byte g = (data & 0x0F00) >> 8;	g |= g << 4;
				//Byte b = (data & 0x00F0) >> 4;	b |= b << 4;
				//Byte a = (data & 0x000F) >> 0;	a |= a << 4;

				Byte r = (data & 0xC0) >> 6;	r |= r << 2 | r << 4 | r << 6;
				Byte g = (data & 0x30) >> 4;	g |= g << 2 | g << 4 | g << 6;
				Byte b = (data & 0x0C) >> 2;	b |= b << 2 | b << 4 | b << 6;
				Byte a = (data & 0x03) >> 0;	a |= a << 2 | a << 4 | a << 6;

				SDL_Rect dst = { x, y, 16, 16 };
				SDL_SetRenderDrawColor(gfx->Renderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawRect(gfx->Renderer(), &dst);
				dst.x = x + 1;
				dst.y = y + 1;
				dst.w = 15;
				dst.h = 15;
				SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawRect(gfx->Renderer(), &dst);
				dst.x = x + 1;
				dst.y = y + 1;
				dst.w = 14;
				dst.h = 14;
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, SDL_ALPHA_OPAQUE);
				SDL_RenderFillRect(gfx->Renderer(), &dst);

				//SDL_RenderDrawPoint(gfx->Renderer(), x, y);
			}
		}
	}
}

void GfxTile::OnRender()
{
	SDL_SetRenderTarget(gfx->Renderer(), NULL);
	if (gfx->Fullscreen())
	{
		int ww, wh;
		SDL_GetWindowSize(gfx->Window(), &ww, &wh);
		float fh = (float)wh;
		float fw = fh * gfx->Aspect();
		if (fw > ww)
		{
			fw = (float)ww;
			fh = fw / gfx->Aspect();
		}
		SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
		SDL_RenderCopy(gfx->Renderer(), _tile_texture, NULL, &dest);
	}
	else
		SDL_RenderCopy(gfx->Renderer(), _tile_texture, NULL, NULL);
}

// * GfxTile16.h ***************************************
// *
// *  16x16 Tile Mode
// ************************************

#include "types.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxTile16.h"


// Graphics Mode Unique Callback Function:
Byte GfxTile16::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		printf("GfxTile16::OnCallback() -- READ\n");
	}
	else
	{	// WRITE
		printf("GfxTile16::OnCallback() -- WRITE\n");
	}
	return data;
}

GfxTile16::GfxTile16()
{
	//printf("GfxTile16::GfxTile16()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;

	pixel_width = 512;
	pixel_height = 320;
}

void GfxTile16::OnInitialize()
{
}

void GfxTile16::OnActivate()
{
}
void GfxTile16::OnDeactivate()
{
}

void GfxTile16::OnQuit()
{
}

void GfxTile16::OnCreate()
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
void GfxTile16::OnDestroy()
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


void GfxTile16::OnUpdate(float fElapsedTime)
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
				Word data = bus->debug_read_word(ofs++);
				if (ofs > VIDEO_END)
					ofs = VIDEO_START;

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

void GfxTile16::OnRender()
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

/**** GfxSprite.cpp ***************************************
 *
 *  16x16 Sprite Container Mode
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#include "types.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxSprite.h"

 // Graphics Mode Unique Callback Function:
Byte GfxSprite::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		//printf("GfxTile16::OnCallback() -- READ\n");
	}
	else
	{	// WRITE
		//printf("GfxTile16::OnCallback() -- WRITE\n");
	}
	return data;
}

GfxSprite::GfxSprite()
{
	//printf("GfxSprite::GfxSprite()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

void GfxSprite::OnInitialize()
{
	if (palette256.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
			{ 0x0000 },	// 0000 0000.0000 1111		0
			{ 0x005F },	// 0000 0000.0101 1111		1
			{ 0x050F },	// 0000 0101.0000 1111		2
			{ 0x055F },	// 0000 0101.0101 1111		3
			{ 0x500F },	// 0101 0000.0000 1111		4
			{ 0x505F },	// 0101 0000.0101 1111		5
			{ 0x550F },	// 0101 0101.0000 1111		6
			{ 0xAAAF },	// 1010 1010.1010 1111		7
			{ 0x555F },	// 0101 0101.0101 1111		8
			{ 0x00FF },	// 0000 0000.1111 1111		9
			{ 0x0F0F },	// 0000 1111.0000 1111		a
			{ 0x0FFF },	// 0000 1111.1111 1111		b
			{ 0xF00F },	// 1111 0000.0000 1111		c
			{ 0xF0FF },	// 1111 0000.1111 1111		d
			{ 0xFF0F },	// 1111 1111.0000 1111		e
			{ 0xFFFF },	// 1111 1111.1111 1111		f
		};
		for (int t = 0; t < 16; t++)
			palette256.push_back(ref[t]);

		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.g = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.g = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.g = t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.g = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.g = 15 - t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.g = 15 - t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.g = t;
			ent.b = 15 - t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.g = 15 - t;
			ent.b = 15 - t;
			palette256.push_back(ent);
		}
		// fill out the remaining entries with random junk for now
		Word color = 0x0010;
		while (palette256.size() < 256)
		{
			GFX::PALETTE ent;
			color += 0x2340;	// rand() % 0x10000;
			ent.color = color;
			palette256.push_back(ent);
		}
	}
}

void GfxSprite::OnActivate()
{
}
void GfxSprite::OnDeactivate()
{
}

void GfxSprite::OnQuit()
{
}

void GfxSprite::OnCreate()
{
	//printf("GfxSprite::OnCreate()\n");

	// create the sprite textures
	// ...

	// not yet sure what to do here... const low res, const hi-res, or dynamic?
	// ...

}
void GfxSprite::OnDestroy()
{
	//printf("GfxSprite::OnDestroy()\n");

	// destroy the tile textures
	// ...

	// not yet sure what to do here... const low res, const hi-res, dynamic, or simply individual sprites
	// ...
}


void GfxSprite::OnUpdate(float fElapsedTime)
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
		// ...
	}
}

void GfxSprite::OnRender()
{
	/*** will need to be revised to render sprites not a screen ***
	

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


	*****************************************************************/

}

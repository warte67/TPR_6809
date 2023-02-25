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

 //		//  Sprite Hardware Registers:
 //		     SPR_BEGIN = 0x182b,        // Start of Sprite Hardware Registers
 //		
 //		//  Sprite Flag Registers:
 //		   SPR_ENABLE = 0x182b,         // (4-Bytes) sprite Enable Bits. 1 bit per sprite
 //		   SPR_COL_ENA = 0x182f,        // (4-Bytes) sprite collision enable. 1 bit per sprite
 //		   SPR_COL_TYP = 0x1833,        // (4-Bytes) sprite collision type (0:hitbox, 1:pixel perfect)
 //		
 //		//  Sprite Palette Registers:
 //		   SPR_PAL_IDX = 0x1837,        // (Byte) color palette index
 //		   SPR_PAL_DAT = 0x1838,        // (Word) indexed sprite palette entry color bits RGBA4444
 //		
 //		//  Sprite Indexed Registers:
 //		   SPR_COL_DET = 0x183a,        // (4-Bytes) Collision detection bits. One bit per colliding sprite.
 //		     SPR_H_POS = 0x183e,        // (Sint16) signed 16-bit integer
 //		     SPR_V_POS = 0x1840,        // (Sint16) signed 16-bit integer
 //		     SPR_X_OFS = 0x1842,        // (Sint8) signed 8-bit integer horizontal display offset
 //		     SPR_Y_OFS = 0x1843,        // (Sint8) signed 8-bit integer vertical display offset
 //		      SPR_PRIO = 0x1844,        // (Byte) Sprite Display Priority:
 //		                                //      0) displays directly behind all foreground modes
 //		                                //      1) displays infront of Glyph32 layer 0 but all other foreground modes
 //		                                //      2) displays infront of Glyph32 layer 1 but all other foreground modes
 //		                                //      3) displays infront of Glyph32 layer 2 but all other foreground modes
 //		                                //      4) displays infront of Glyph32 layer 3 but all other foreground modes
 //		                                //      5) displays infront of Debug layer, but behind the mouse cursor
 //		                                //      6) displays infront of Mouse Cursor layer (in index order)
 //		                                //      7) displays in sprite order
 //		
 //		//  Sprite Indexed Bitmap Pixel Data:
 //		   SPR_BMP_IDX = 0x1845,        // (Byte) Sprite pixel offset (Y*16+X)
 //		   SPR_BMP_DAT = 0x1846,        // (Byte) Sprite color palette index data
 //		
 //		//  End of Sprite Hardware Registers
 //		       SPR_END = 0x1846,        // End of the Sprite Hardware Registers
 //		

 // Graphics Mode Unique Callback Function:
Byte GfxSprite::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	//printf("GfxSprite::OnCallback($%04X)\n", ofs);

	// flag enable registers (SPR_ENABLE, SPR_COL_ENA, and SPR_COL_TYP)
	Uint32 wb = (3 - (ofs - SPR_ENABLE)) * 8;
	if (ofs >= SPR_ENABLE && ofs <= SPR_ENABLE + 3)
	{
		if (bWasRead)	
			return (spr_enable >> wb) & 0xFF;
		else
		{	// WRITE
			Uint32 mask = 0xFF << wb;
			spr_enable &= ~mask;			// AND out old byte
			spr_enable |= (data << wb);		// OR in new data							
			bus->debug_write( ofs, data	);
			printf("SPR_ENABLE: $%08X\n", spr_enable);
		}
	}
	if (ofs >= SPR_COL_ENA && ofs <= SPR_COL_ENA + 3)
	{
		if (bWasRead)
			return (spr_col_ena >> wb) & 0xFF;
		else
		{	// WRITE
			Uint32 mask = 0xFF << wb;
			spr_col_ena &= ~mask;			// AND out old byte
			spr_col_ena |= (data << wb);	// OR in new data							
			bus->debug_write(ofs, data);
			printf("SPR_COL_ENA: $%08X\n", spr_col_ena);
		}
	}
	if (ofs >= SPR_COL_TYP && ofs <= SPR_COL_TYP + 3)
	{
		if (bWasRead)
			return (spr_col_typ >> wb) & 0xFF;
		else
		{	// WRITE
			Uint32 mask = 0xFF << wb;
			spr_col_typ &= ~mask;			// AND out old byte
			spr_col_typ |= (data << wb);		// OR in new data							
			bus->debug_write(ofs, data);
			printf("SPR_COL_TYP: $%08X\n", spr_col_typ);
		}
	}





	return data;
}

Word GfxSprite::MapDevice(MemoryMap* memmap, Word offset)
{
	//printf("GfxSprite::MapDevice()\n");

	std::string reg_name = "Sprite System";
	DWord st_offset = offset;

	// map fundamental Debugger hardware registers:
	//memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Sprite Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "SPR_BEGIN", "Start of Sprite Hardware Registers" }); offset += 0;
	memmap->push({ offset, "", "" }); offset += 0;

	memmap->push({ offset, "", "Sprite Flag Registers:" }); offset += 0;
	memmap->push({ offset, "SPR_ENABLE",	"(4-Bytes) sprite Enable Bits. 1 bit per sprite" }); offset += 4;
	memmap->push({ offset, "SPR_COL_ENA",	"(4-Bytes) sprite collision enable. 1 bit per sprite" }); offset += 4;
	memmap->push({ offset, "SPR_COL_TYP",	"(4-Bytes) sprite collision type (0:hitbox, 1:pixel perfect)" }); offset += 4;
	memmap->push({ offset, "", "" }); offset += 0;

	memmap->push({ offset, "", "Sprite Palette Registers:" }); offset += 0;
	memmap->push({ offset, "SPR_PAL_IDX",	"(Byte) color palette index" }); offset += 1;
	memmap->push({ offset, "SPR_PAL_DAT",	"(Word) indexed sprite palette entry color bits RGBA4444" }); offset += 2;
	memmap->push({ offset, "", "" }); offset += 0;

	memmap->push({ offset, "", "Sprite Indexed Registers:" }); offset += 0;
	memmap->push({ offset, "SPR_COL_DET",	"(4-Bytes) Collision detection bits. One bit per colliding sprite." }); offset += 4;
	memmap->push({ offset, "SPR_H_POS",		"(Sint16) signed 16-bit integer" }); offset += 2;
	memmap->push({ offset, "SPR_V_POS",		"(Sint16) signed 16-bit integer" }); offset += 2;
	memmap->push({ offset, "SPR_X_OFS",		"(Sint8) signed 8-bit integer horizontal display offset" }); offset += 1;
	memmap->push({ offset, "SPR_Y_OFS",		"(Sint8) signed 8-bit integer vertical display offset" }); offset += 1;
	memmap->push({ offset, "SPR_PRIO",		"(Byte) Sprite Display Priority:" }); offset += 1;
	memmap->push({ offset, "",				">    0) displays directly behind all foreground modes" }); offset += 0;
	memmap->push({ offset, "",				">    1) displays infront of Glyph32 layer 0 but all other foreground modes" }); offset += 0;
	memmap->push({ offset, "",				">    2) displays infront of Glyph32 layer 1 but all other foreground modes" }); offset += 0;
	memmap->push({ offset, "",				">    3) displays infront of Glyph32 layer 2 but all other foreground modes" }); offset += 0;
	memmap->push({ offset, "",				">    4) displays infront of Glyph32 layer 3 but all other foreground modes" }); offset += 0;
	memmap->push({ offset, "",				">    5) displays infront of Debug layer, but behind the mouse cursor" }); offset += 0;
	memmap->push({ offset, "",				">    6) displays infront of Mouse Cursor layer (in index order)" }); offset += 0;
	memmap->push({ offset, "",				">    7) displays in sprite order" }); offset += 0;
	memmap->push({ offset, "", "" }); offset += 0;

	memmap->push({ offset, "", "Sprite Indexed Bitmap Pixel Data:" }); offset += 0;
	memmap->push({ offset, "SPR_BMP_IDX",	"(Byte) Sprite pixel offset (Y*16+X)" }); offset += 1;
	memmap->push({ offset, "SPR_BMP_DAT",	"(Byte) Sprite color palette index data" }); offset += 1;

	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "End of Sprite Hardware Registers" }); offset += 0;
	memmap->push({ --offset, "SPR_END", "End of the Sprite Hardware Registers" }); offset += 1;

	return offset - st_offset;
}

GfxSprite::GfxSprite()
{
	//printf("GfxSprite::GfxSprite()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}
GfxSprite::~GfxSprite()
{
	//printf("GfxSprite::~GfxSprite()\n");
}


void GfxSprite::OnInitialize()
{
	//printf("GfxSprite::OnInitialize()\n");

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
	//printf("GfxSprite::OnActivate()\n");
}
void GfxSprite::OnDeactivate()
{
	//printf("GfxSprite::OnDeactivate()\n");
}

void GfxSprite::OnQuit()
{
	//printf("GfxSprite::OnQuit()\n");
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
	//printf("GfxSprite::OnUpdate()\n");


	/*** will need to be revised to render sprites not a screen ***

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

	*****************************************************************/
}

void GfxSprite::OnRender()
{
	//printf("GfxSprite::OnRender()\n");

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

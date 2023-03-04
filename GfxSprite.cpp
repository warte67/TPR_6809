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


//		 //  Sprite Hardware Registers:
//		SPR_BEGIN = 0x182b,        // Start of Sprite Hardware Registers
//		
//		//  Sprite Flag Registers:
//		SPR_ENABLE = 0x182b,        // (4-Bytes) sprite Enable Bits. 1 bit per sprite
//		SPR_COL_ENA = 0x182f,        // (4-Bytes) sprite collision enable. 1 bit per sprite
//		SPR_COL_TYP = 0x1833,        // (4-Bytes) sprite collision type (0:hitbox, 1:pixel perfect)
//		
//		//  Sprite Palette Registers:
//		SPR_PAL_INDX = 0x1837,        // (Byte) color palette index
//		SPR_PAL_DATA = 0x1838,        // (Word) indexed sprite palette entry color bits RGBA4444
//		
//		//  Sprite Index Register:
//		SPR_INDEX = 0x183a,        // (Byte) 0-31 indexes the 'current' sprite
//		
//		//  Indexed Sprite Registers:
//		SPR_COL_DET = 0x183b,        // (4-Bytes) Collision detection bits. One bit per colliding sprite.
//		SPR_H_POS = 0x183f,        // (Sint16) signed 16-bit integer
//		SPR_V_POS = 0x1841,        // (Sint16) signed 16-bit integer
//		SPR_X_OFS = 0x1843,        // (Sint8) signed 8-bit integer horizontal display offset
//		SPR_Y_OFS = 0x1844,        // (Sint8) signed 8-bit integer vertical display offset
//		SPR_PRIO = 0x1845,        // (Byte) Sprite Display Priority:
//		//      0) displays directly behind all foreground modes
//		//      1) displays infront of Glyph32 layer 0 but all other foreground modes
//		//      2) displays infront of Glyph32 layer 1 but all other foreground modes
//		//      3) displays infront of Glyph32 layer 2 but all other foreground modes
//		//      4) displays infront of Glyph32 layer 3 but all other foreground modes
//		//      5) displays infront of Debug layer, but behind the mouse cursor
//		//      6) displays infront of Mouse Cursor layer (in index order)
//		//      7) displays in sprite order
//		
//		//  Sprite Indexed Bitmap Pixel Data:
//		SPR_BMP_INDX = 0x1846,        // (Byte) Sprite pixel offset (Y*16+X)
//		SPR_BMP_DATA = 0x1847,        // (Byte) Sprite color palette index data
//		
//		//  End of Sprite Hardware Registers
//		SPR_END = 0x1847,        // End of the Sprite Hardware Registers
//		

 // Graphics Mode Unique Callback Function:
Byte GfxSprite::OnCallback(GfxMode* memDev, Word ofs, Byte data, bool bWasRead)
{
	//printf("GfxSprite::OnCallback($%04X)\n", ofs);

	Bus* bus = Bus::getInstance();
	//GfxSprite* ptrSprite = dynamic_cast<GfxSprite*>(memDev);

	// flag enable registers (SPR_ENABLE, SPR_COL_ENA, and SPR_COL_TYP)	
	if (ofs >= SPR_ENABLE && ofs <= SPR_ENABLE + 3)
	{
		Uint32 wb = (3 - (ofs - SPR_ENABLE)) * 8;
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
		Uint32 wb = (3 - (ofs - SPR_ENABLE)) * 8;
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
		Uint32 wb = (3 - (ofs - SPR_ENABLE)) * 8;
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

	//  Sprite Palette Registers:
	if (ofs == SPR_PAL_INDX)
	{
		if (bWasRead)
		{	// READ
			bus->debug_write(ofs, data);
			return m_palette_index = data;
		}
		else
		{	// WRITE
			bus->debug_write(ofs, data);
			m_palette_index = data;
		}
	}
	if (ofs >= SPR_PAL_DATA && ofs <= SPR_PAL_DATA + 1)
	{
		if (bWasRead)
		{	// READ
			if (ofs == SPR_PAL_DATA)
			{
				data = palette256[m_palette_index].color >> 8;
				bus->debug_write(ofs, data);
				return data;
			}
			if (ofs == SPR_PAL_DATA + 1)
			{
				data = palette256[m_palette_index].color & 0xFF;
				bus->debug_write(ofs, data);
				return data;
			}
		}
		else
		{	// WRITE
			if (ofs == SPR_PAL_DATA)
			{
				bus->debug_write(ofs, data);
				palette256[m_palette_index].color =
					(palette256[m_palette_index].color & 0x00FF) | (data << 8);
				// FORCE REFRESH
				// bIsDirty = true;
			}
			if (ofs == SPR_PAL_DATA + 1)
			{
				bus->debug_write(ofs, data);
				palette256[m_palette_index].color =
					(palette256[m_palette_index].color & 0xFF00) | (data << 0);
				// FORCE REFRESH
				// bIsDirty = true;
			}
		}
	}

	// sprite index register
	if (ofs == SPR_INDEX)
	{
		if (bWasRead)
		{	// READ
			data = spr_index % SPRITE_MAX;
			bus->debug_write(ofs, data);
			return data;
		}
		else
		{	// WRITE
			spr_index = data % SPRITE_MAX;
			bus->debug_write(ofs, data);
		}
	}

	// indexed sprite registers
	if (ofs >= SPR_COL_DET && ofs <= SPR_COL_DET + 3)
	{
		Uint32 wb = (3 - (ofs - SPR_COL_DET)) * 8;
		if (bWasRead)
		{	// READ
			data = (spr_col_det[spr_index] >> wb) & 0xFF;
			bus->debug_write(ofs, data);
			return data;
		}
		else
		{	// WRITE
			Uint32 mask = 0xFF << wb;
			spr_col_det[spr_index] &= ~mask;			// AND out old byte
			spr_col_det[spr_index] |= (data << wb);		// OR in new data							
			bus->debug_write(ofs, data);
			printf("SPR_COL_DET[$%02X]: $%08X\n", spr_index, spr_col_det[spr_index]);
		}
	}	
	if (ofs >= SPR_H_POS && ofs <= SPR_H_POS + 1)
	{
		if (bWasRead)
		{	// READ
			if (ofs == SPR_H_POS)
			{	// Most Significant Byte
				data = (spr_h_pos[spr_index] >> 8) & 0xFF;
				bus->debug_write(ofs, data);
				return data;
			}
			if (ofs == SPR_H_POS + 1)
			{	// Least Significant Byte
				data = (spr_h_pos[spr_index] >> 0) & 0xFF;
				bus->debug_write(ofs, data);
				return data;
			}
		}
		else
		{	// WRITE
			if (ofs == SPR_H_POS)
			{	// Most Significant Byte					
				spr_h_pos[spr_index] &= 0x00ff;
				spr_h_pos[spr_index] |= (data << 8);
				bus->debug_write(ofs, data);
			}
			if (ofs == SPR_H_POS + 1)
			{	// Least Significant Byte
				spr_h_pos[spr_index] &= 0xff00;
				spr_h_pos[spr_index] |= (data << 0);
				bus->debug_write(ofs, data);
			}
		}
	}
	if (ofs >= SPR_V_POS && ofs <= SPR_V_POS + 1)
	{
		if (bWasRead)
		{	// READ
			if (ofs == SPR_V_POS)
			{	// Most Significant Byte
				data = (spr_v_pos[spr_index] >> 8) & 0xFF;
				bus->debug_write(ofs, data);
				return data;
			}
			if (ofs == SPR_V_POS + 1)
			{	// Least Significant Byte
				data = (spr_v_pos[spr_index] >> 0) & 0xFF;
				bus->debug_write(ofs, data);
				return data;
			}
		}
		else
		{	// WRITE
			if (ofs == SPR_V_POS)
			{	// Most Significant Byte					
				spr_v_pos[spr_index] &= 0x00ff;
				spr_v_pos[spr_index] |= (data << 8);
				bus->debug_write(ofs, data);
			}
			if (ofs == SPR_V_POS + 1)
			{	// Least Significant Byte
				spr_v_pos[spr_index] &= 0xff00;
				spr_v_pos[spr_index] |= (data << 0);
				bus->debug_write(ofs, data);
			}
		}
	}
	if (ofs == SPR_X_OFS || ofs == SPR_Y_OFS || ofs == SPR_PRIO)
	{
		if (bWasRead)
		{	// READ
			if (ofs == SPR_X_OFS)
				data = spr_x_ofs[spr_index];
			if (ofs == SPR_Y_OFS)
				data = spr_y_ofs[spr_index];
			if (ofs == SPR_PRIO)
				data = spr_prio[spr_index];
			bus->debug_write(ofs, data);
		}
		else
		{	// WRITE 
			bus->debug_write(ofs, data);
			if (ofs == SPR_X_OFS)
				spr_x_ofs[spr_index] = data;
			if (ofs == SPR_Y_OFS)
				spr_y_ofs[spr_index] = data;
			if (ofs == SPR_PRIO)
				spr_prio[spr_index] = data;
		}
	}

	//  Indexed Sprite Indexed Bitmap Pixel Data
	if (ofs == SPR_BMP_INDX || ofs == SPR_BMP_DATA)
	{
		if (bWasRead)
		{	// READ
			if (ofs == SPR_BMP_INDX)
				data = spr_bmp_index[spr_index];
			if (ofs == SPR_BMP_DATA)
				data = spr_bmp_data[spr_index][spr_bmp_index[spr_index]];
			bus->debug_write(ofs, data);
		}
		else
		{	// WRITE
			if (ofs == SPR_BMP_INDX)
				spr_bmp_index[spr_index] = data;
			if (ofs == SPR_BMP_DATA)
				spr_bmp_data[spr_index][spr_bmp_index[spr_index]] = data;
			bus->debug_write(ofs, data);
			// FORCE REFRESH
			// bIsDirty = true;
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
	memmap->push({ offset, "SPR_PAL_INDX",	"(Byte) color palette index" }); offset += 1;
	memmap->push({ offset, "SPR_PAL_DATA",	"(Word) indexed sprite palette entry color bits RGBA4444" }); offset += 2;
	memmap->push({ offset, "", "" }); offset += 0;

	memmap->push({ offset, "", "Sprite Index Register:" }); offset += 0;
	memmap->push({ offset, "SPR_INDEX",	"(Byte) 0-31 indexes the 'current' sprite" }); offset += 1;
	memmap->push({ offset, "", "" }); offset += 0;

	memmap->push({ offset, "", "Indexed Sprite Registers:" }); offset += 0;
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
	memmap->push({ offset, "SPR_BMP_INDX",	"(Byte) Sprite pixel offset (Y*16+X)" }); offset += 1;
	memmap->push({ offset, "SPR_BMP_DATA",	"(Byte) Sprite color palette index data" }); offset += 1;

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


void GfxSprite::OnInitialize()		// -> OnQuit()
{
	//printf("GfxSprite::OnInitialize()\n");

	// create 32 blank sprites
	for (int t = 0; t < SPRITE_MAX; t++)
	{
		Sprite* newSprite = new Sprite(this);
		newSprite->bus = Bus::getInstance();
		newSprite->gfx = bus->m_gfx;
		sprites.push_back(newSprite);
	}

	// create a default 256-color palette
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

void GfxSprite::OnActivate()		// <- OnDeactivate()
{
	//printf("GfxSprite::OnActivate()\n");
}
void GfxSprite::OnDeactivate()		// -> OnActivate()
{
	//printf("GfxSprite::OnDeactivate()\n");
}

void GfxSprite::OnQuit()			// <- OnInitialize()
{
	//printf("GfxSprite::OnQuit()\n");

	// destroy 32 sprites
	for (int t = 0; t < sprites.size(); t++)
		delete sprites[t];
	sprites.clear();
}

void GfxSprite::OnCreate()			// -> OnDestroy()
{
	//printf("GfxSprite::OnCreate()\n");
}
void GfxSprite::OnDestroy()			// <- OnCreate()
{
	//printf("GfxSprite::OnDestroy()\n");
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






// **** SPRITE METHOD IMPLEMENTAION ******************************************

Sprite::Sprite(GfxSprite* p)
{
	//printf("Sprite::Sprite()\n");
	gfx_sprite = p;
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
	OnCreate();
}
Sprite::~Sprite()
{
	//printf("Sprite::~Sprite()\n");
	this->OnDestroy();
}
void Sprite::OnCreate()
{
	//printf("Sprite::OnCreate()\n");
}
void Sprite::OnDestroy()
{
	//printf("Sprite::OnDestroy()\n");
}



// ***** NOTE: these two may not be needed. **************
void Sprite::OnActivate()
{
	printf("Sprite::OnActivate()\n");
}
void Sprite::OnDeactivate()
{
	printf("Sprite::OnDeactivate()\n");
}
// ***** NOTE: these two may not be needed. **************




void Sprite::OnUpdate(float fElapsedTime)
{
}
void Sprite::OnRender()
{
}
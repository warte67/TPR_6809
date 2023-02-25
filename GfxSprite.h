/**** GfxSprite.h ***************************************
 *
 *  16x16 Sprite Container Mode
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#pragma once
#ifndef __GFXSPRITE_H__
#define __GFXSPRITE_H__

class Sprite;


class GfxSprite : public GfxMode
{
public:
	GfxSprite();
	virtual ~GfxSprite();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;
	static Word MapDevice(MemoryMap* memmap, Word offset);
	// static members


	virtual void OnInitialize() override;				// runs once after all devices are created
	virtual void OnQuit() override;						// fires on exit -- reverses OnInitialize()
	//virtual void OnEvent(SDL_Event* evnt) override;		// fires per SDL_Event
	virtual void OnCreate() override;					// fires when the object is created/recreated
	virtual void OnDestroy() override;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) override;	// fires each frame, for updates
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnRender() override;

private:

	SDL_Texture* _tile_texture = nullptr;
	std::vector<GFX::PALETTE> default_palette;

	std::vector<GFX::PALETTE> palette256;
	static Uint8 m_palette_index;
	Uint8 red(Uint8 index) { Uint8 c = palette256[index].r;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 grn(Uint8 index) { Uint8 c = palette256[index].g;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 blu(Uint8 index) { Uint8 c = palette256[index].b;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 alf(Uint8 index) { Uint8 c = palette256[index].a;  return c | (c << 4) | (c << 8) | (c << 12); }

};



class Sprite
{

};


#endif // __GFXSPRITE_H__




//		//  Sprite Hardware Registers:
//		     SPR_BEGIN = 0x182b,        // Start of Sprite Hardware Registers
//		
//		//  Sprite Flag Registers:
//		   SPR_DIS_ENA = 0x182b,        // (4-Bytes) sprite Enable Bits. 1 bit per sprite
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



/**** SPRITE SUB-SYSTEM NOTES: ****************************************************

	The Sprite System supports 32 x 16x16x256 color sprite objects. The 8K sprite
	bitmap table is stored in the GfxMode::s_mem_64k[$$E000-$FFFF] array buffer. 


	Static Registers:
		Sprite Flags:
			SPR_DIS_ENA			(4-Bytes) Sprite Enable Bits. 1 bit per sprite
			SPR_COL_ENA			(4-Bytes) Sprite Collision Enable. 1 bit per sprite
			SPR_COL_TYP			(4-Bytes) Sprite Collision Type (0:hitbox, 1:pixel perfect)

		Sprite Palette:
			SPR_PAL_IDX		(Byte) Color Palette Index
			SPR_PAL_DAT		(Word) sprite palette color bits RGBA4444

		Sprite Index Register:
			SPR_INDEX		(Byte) 0-31 indexes the "current" sprite

		Sprite Indexed Registers:
			SPR_COL_DET		(4-Bytes) Collision detection bits. One bit per colliding sprite
			SPR_PRIO		(Byte) Sprite Display Priority:
								0) displays directly behind all foreground modes
								1) displays infront of Glyph32 layer 0 but all other foreground modes
								2) displays infront of Glyph32 layer 1 but all other foreground modes
								3) displays infront of Glyph32 layer 2 but all other foreground modes
								4) displays infront of Glyph32 layer 3 but all other foreground modes
								5) displays infront of Debug layer, but behind the mouse cursor
								6) displays infront of Mouse Cursor layer (in index order)
								7) displays in sprite order
			
			SPR_H_POS		(Sint16) signed 16-bit integer
			SPR_V_POS		(Sint16) signed 16-bit integer
			SPR_X_OFS		(Sint8) signed 8-bit integer horizontal display offset
			SPR_Y_OFS		(Sint8) signed 8-bit integer vertical display offset

		Sprite Indexed Bitmap Pixel Data:
			SPR_BMP_IDX		(Byte) Sprite pixel offset (Y*16+X)
			SPR_BMP_DAT		(Byte) Sprite color palette index data

************************************************************************/
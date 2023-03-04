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

// sprite container class
class GfxSprite : public GfxMode
{
public:
	GfxSprite();
	virtual ~GfxSprite();

	virtual Byte OnCallback(GfxMode* memDev, Word ofs, Byte data, bool bWasRead)  override;
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

	// bool bIsDirty = true;

	SDL_Texture* _tile_texture = nullptr;
	std::vector<GFX::PALETTE> default_palette;

	std::vector<GFX::PALETTE> palette256;
	Uint8 m_palette_index;
	Uint8 red(Uint8 index) { Uint8 c = palette256[index].r;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 grn(Uint8 index) { Uint8 c = palette256[index].g;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 blu(Uint8 index) { Uint8 c = palette256[index].b;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 alf(Uint8 index) { Uint8 c = palette256[index].a;  return c | (c << 4) | (c << 8) | (c << 12); }

// hardware registers
private:
	Uint32 spr_enable	= 0;		// 32-sprite bit-fields
	Uint32 spr_col_ena	= 0;		// 32-sprite bit-fields
	Uint32 spr_col_typ	= 0;		// 32-sprite bit-fields

#define SPRITE_MAX 32				// bitfields restrict sprites to 32 total
	Byte spr_index = 0;				// current sprite index (0-31)

	// this data should be stored within the individual sprites (move 'em there)
	Uint32 spr_col_det[SPRITE_MAX]	{ 0 };		// 32-bit sprite bitfields
	Sint16 spr_h_pos[SPRITE_MAX]	{ 0 };		// 16-bit signed horizontal position
	Sint16 spr_v_pos[SPRITE_MAX]	{ 0 };		// 16-bit signed vertical position
	Sint8 spr_x_ofs[SPRITE_MAX]		{ 0 };		// 8-bit signed horizontal offset
	Sint8 spr_y_ofs[SPRITE_MAX]		{ 0 };		// 8-bit signed vertical offset
	Uint8 spr_prio[SPRITE_MAX]		{ 0 };		// 8-bit unsigned sprite priority

	// move these to the individual Sprite objects contained within the sprites vector
	Byte spr_bmp_index[SPRITE_MAX]		{ 0 };	// sprite bitmap pixel index (Y*16+X)
	Byte spr_bmp_data[SPRITE_MAX][256]	{ 0 };	// sprite bitmap color data

	// Sprite data
	std::vector<Sprite*> sprites;
};


// The sprite class contains the individual sprites and their actual texture image data
class Sprite
{
	friend GfxSprite;

public:
	Sprite(GfxSprite* p);
	virtual ~Sprite();

	// Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;

	// void OnEvent(SDL_Event* evnt);	// fires per SDL_Event
	void OnCreate();					// fires when the object is created/recreated
	void OnDestroy();					// fires when the object is destroyed/lost focus
	void OnActivate();
	void OnDeactivate();
	void OnUpdate(float fElapsedTime);	// fires each frame, for updates
	void OnRender();

private:
	Uint32 spr_col_det = 0;		// 32-bit sprite bitfields
	Sint16 spr_h_pos = 0;		// 16-bit signed horizontal position
	Sint16 spr_v_pos = 0;		// 16-bit signed vertical position
	Sint8 spr_x_ofs = 0;		// 8-bit signed horizontal offset
	Sint8 spr_y_ofs = 0;		// 8-bit signed vertical offset
	Uint8 spr_prio = 0;			// 8-bit unsigned sprite priority

	Byte spr_bmp_index = 0;		// sprite bitmap pixel index (Y*16+X)
	Byte spr_bmp_data[256] { 0 };	// sprite bitmap color data


	GfxSprite* gfx_sprite = nullptr;
	Bus* bus = nullptr;
	GFX* gfx = nullptr;
};


#endif // __GFXSPRITE_H__

/**** NOTES: *******************************************************

	The GfxImage device supports 64 images while in the 256-Indexed display mode or
	up to 256 images otherwise. These images are split between sprites and tiles.
	The Indexed display mode will use the first 192 images as a display buffer. 

	The current Sprite Flags mechanism will no longer work after implementing a 
	more dynamic image allocation strategy. SPR_ENABLE, SPR_COL_ENA, and SPR_COL_TYP
	should be revised to a per sprite/tile implementation rather than using global flags.

********************************************************************/

/**** SPRITE SUB-SYSTEM NOTES: ****************************************************

	The Sprite System supports 32 x 16x16x256 color sprite objects. The 8K sprite
	bitmap table is stored in the GfxMode::s_mem_64k[$$E000-$FFFF] array buffer. 


	Static Registers:
		Sprite Flags:
			SPR_ENABLE			(4-Bytes) Sprite Enable Bits. 1 bit per sprite
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
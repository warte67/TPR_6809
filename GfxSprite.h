/**** GfxSprite.h ***************************************
 *
 *  16x16 Sprite Container Mode
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#pragma once
#ifndef __GFXSPRITE_H__
#define __GFXSPRITE_H__


class GfxSprite : public GfxMode
{
public:
	GfxSprite();
	virtual ~GfxSprite() {}

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;
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


#endif // __GFXSPRITE_H__






/**** SPRITE SUB-SYSTEM NOTES: ****************************************************

	The Sprite System supports 32 x 16x16x256 color sprite objects. The 8K sprite bitmap table is stored
	in the GfxMode::s_mem_64k[$$E000-$FFFF] array buffer. 


	Static Registers:
		Sprite Palette:
			SPR_PAL_IDX		(Byte) Color Palette Index
			SPR_PAL_DAT		(Word) sprite palette color bits RGBA4444

		Sprite Index Register:
			SPR_INDEX		(Byte) 0-31 indexes the "current" sprite.

	Indexed Sprite Registers:   (This references a specific Sprite object node)
		SPR_FLAGS		(Byte) Sprite State Flags:
							bit 0: display enable (0:disabled, 1:enabled)
							bit 1: collision enable (0:disabled, 1:enabled)
							bit 2: collision type (0:hitbox, 1:pixel perfect)
							bit 3-7: reserved

		SPR_COL_DET		(4-Bytes) Collision detection bits. One bit per sprite colliding with this one.
		SPR_H_POS		(Sint16) signed 16-bit integer
		SPR_V_POS		(Sint16) signed 16-bit integer
		SPR_X_OFS		(Sint8) signed 8-bit integer horizontal display offset
		SPR_Y_OFS		(Sint8) signed 8-bit integer vertical display offset
		SPR_PRIO		(Byte)	display order:
							0) displays directly infront of the background mode, but behind all foreground modes
							1) displays infront of Glyph32 layer 0	and all other foreground modes
							2) displays infront of Glyph32 layer 1	and all other foreground modes
							3) displays infront of Glyph32 layer 2	and all other foreground modes
							4) displays infront of Glyph32 layer 3	and all other foreground modes
							5) displays infront of Debug layer, but behind the mouse cursor
							6) displays infront of Mouse Cursor layer
							7+) displays in priority order then in sprite order (where priority is equal)

		SPR_BMP_IDX		(Byte) Sprite pixel offset (Y*16+X)
		SPR_BMP_DAT		(Byte) Sprite color palette index data

************************************************************************/
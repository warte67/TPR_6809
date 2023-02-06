/**** GfxTile16.h ***************************************
 *
 *  16x16 Tile Container Mode
 * 
 *  Copyright (C) 2023 by Jay Faries
 ************************************/
#pragma once
#ifndef __GFXTILE_H__
#define __GFXTILE_H__

class GfxTile16 : public GfxMode
{
	friend class GfxTile32;
public:
	GfxTile16();
	virtual ~GfxTile16() {}

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;

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
	int pixel_width;
	int pixel_height;

	std::vector<GFX::PALETTE> palette256;
	static Uint8 m_palette_index;
	Uint8 red(Uint8 index) { Uint8 c = palette256[index].r;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 grn(Uint8 index) { Uint8 c = palette256[index].g;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 blu(Uint8 index) { Uint8 c = palette256[index].b;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 alf(Uint8 index) { Uint8 c = palette256[index].a;  return c | (c << 4) | (c << 8) | (c << 12); }

};

#endif // __GFXTILE_H__
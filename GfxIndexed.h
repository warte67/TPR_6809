// * GfxIndexed.h ***************************************
// *
// * 128x80 x 256-Color Indexed (10KB) - Serial Buffer
// ************************************
#ifndef __GFXINDEXED_H__
#define __GFXINDEXED_H__
#pragma once



#include "GfxMode.h"

class GfxIndexed : public GfxMode
{
public:
	GfxIndexed();
	virtual ~GfxIndexed();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;

	virtual void OnInitialize() override;
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float fElapsedTime) override;
	virtual void OnRender() override;
private:

	SDL_Texture* bitmap_texture = nullptr;
	const int pixel_width = 128;
	const int pixel_height = 80;

	std::vector<GFX::PALETTE> palette256;
	static Uint8 m_palette_index;
	Uint8 red(Uint8 index) { Uint8 c = palette256[index].r;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 grn(Uint8 index) { Uint8 c = palette256[index].g;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 blu(Uint8 index) { Uint8 c = palette256[index].b;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 alf(Uint8 index) { Uint8 c = palette256[index].a;  return c | (c << 4) | (c << 8) | (c << 12); }
};

#endif // __GFXINDEXED_H__


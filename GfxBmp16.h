/**** GfxBmp16.h ***************************************
 *
 *  128x80 x 16-Color BMP Graphics Mode 
 * 
 *  Copyright (C) 2023 by Jay Faries
 ************************************/
#ifndef __GFXBMP16_H__
#define __GFXBMP16_H__
#pragma once


#include "GfxMode.h"
class GfxBmp16 : public GfxMode
{
public:
	GfxBmp16();
	virtual ~GfxBmp16();

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
	std::vector<GFX::PALETTE> default_palette;
	const int pixel_width = 128;
	const int pixel_height = 80;
};




#endif	// __GFXBMP16_H__




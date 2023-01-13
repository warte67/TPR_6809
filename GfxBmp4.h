// * GfxBmp4.h ***************************************
// *
// *  128x160 x 4-Color BMP Graphics Mode 
// ************************************
#ifndef __GFXBMP4_H__
#define __GFXBMP4_H__
#pragma once

#include "GfxMode.h"

class GfxBmp4 : public GfxMode
{
public:
	GfxBmp4();
	virtual ~GfxBmp4();

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
	static const int pixel_width;
	static const int pixel_height;
};


#endif // __GFXBMP4_H__

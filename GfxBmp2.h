// * GfxBmp2.h ***************************************
// *
// *  256x160 x 2-Color BMP Graphics Mode 
// ************************************
#ifndef __GFXBMP2_H__
#define __GFXBMP2_H__
#pragma once
#include "GfxMode.h"


class GfxBmp2 : public GfxMode
{
public:
	GfxBmp2();
	virtual ~GfxBmp2();

	virtual void OnActivate() override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float fElapsedTime) override;
	virtual void OnRender() override;
private:

	SDL_Texture* bitmap_texture = nullptr;
	static const int pixel_width;
	static const int pixel_height;
};

#endif // __GFXBMP2_H__




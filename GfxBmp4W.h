// * GfxBmp4W.h ***************************************
// *
// *  256x80 x 4-Color BMP Graphics Mode 
// ************************************
#pragma once
#ifndef __GFXBMP4W_H__
#define __GFXBMP4W_H__

#include "GfxMode.h"

class GfxBmp4W : public GfxMode
{
public:
	GfxBmp4W();
	virtual ~GfxBmp4W();

	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float fElapsedTime) override;
	virtual void OnRender() override;
private:

	SDL_Texture* bitmap_texture = nullptr;
	static const int pixel_width;
	static const int pixel_height;
};

#endif // __GFXBMP4W_H__

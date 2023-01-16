// * GfxRaw.h ***************************************
// *
// * 128x80 x 4096-Color (16 bpp 20KB) - Serial Buffer / FPGA
// ************************************
#ifndef __GFXRAW_H__
#define __GFXRAW_H__
#pragma once



#include "GfxMode.h"

class GfxRaw : public GfxMode
{
public:
	GfxRaw();
	virtual ~GfxRaw();

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
	static const int pixel_width;
	static const int pixel_height;
};

#endif // __GFXRAW_H__


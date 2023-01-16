// * GfxRaw.h ***************************************
// *
// * 512x320 x 2-Color (1 bpp 20KB) - Serial Buffer / FPGA
// ************************************
#pragma once
#ifndef __GFXHIRES_H__
#define __GFXHIRES_H__


#include "GfxMode.h"
class GfxHires : public GfxMode
{
public:
	GfxHires();
	virtual ~GfxHires();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead) override;

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


#endif	// __GFXHIRES_H__

// * GfxGlyph32.h ***************************************
// *
// *  32x20 Text Glyph Mode 
// *        include FOUR layers of overlayed text
// ************************************

#ifndef __GFXGLYPH32_H__
#define __GFXGLYPH32_H__
#pragma once


class GfxGlyph32 : public GfxMode
{
public:
	GfxGlyph32();
	virtual ~GfxGlyph32() {}

	virtual void OnInitialize() override;				// runs once after all devices are created
	virtual void OnQuit() override;						// fires on exit -- reverses OnInitialize()
	//virtual void OnEvent(SDL_Event* evnt) override;		// fires per SDL_Event
	virtual void OnCreate() override;					// fires when the object is created/recreated
	virtual void OnDestroy() override;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) override;	// fires each frame, for updates
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnRender() override;

	void OutGlyph(int row, int col, Byte glyph, Byte red, Byte grn, Byte blu, bool bDropShadow);

private:

	SDL_Texture* _glyph_texture = nullptr;
	std::vector<SDL_Texture*> glyph_textures;
	std::vector<GFX::PALETTE> default_palette;
};

#endif	// __GFXGLYPH32_H__


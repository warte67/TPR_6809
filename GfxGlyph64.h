// * GfxGlyph64.h ***************************************
// *
// *  64x40 Text Glyph Mode 
// ************************************


#pragma once
#ifndef __GFXGLYPH64_H__
#define __GFXGLYPH64_H__

class GfxGlyph64 : public GfxMode
{
public:
	GfxGlyph64();
	virtual ~GfxGlyph64() {}

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;

	virtual void OnInitialize() override;				// runs once after all devices are created
	virtual void OnQuit() override;						// fires on exit -- reverses OnInitialize()
	//virtual void OnEvent(SDL_Event* evnt) override;	// fires per SDL_Event
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

	Byte font_index = 0;
	Byte glyph_data[256][8] = {};
};

#endif // __GFXGLYPH64_H__
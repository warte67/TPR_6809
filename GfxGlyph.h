// * GfxGlyph.h ***************************************
// *
// *  64x40 Text Glyph Mode 
// ************************************


#pragma once
#ifndef __GFXGLYPH_H__
#define __GFXGLYPH_H__

class GfxGlyph : public GfxMode
{
public:
	GfxGlyph();
	virtual ~GfxGlyph() {}

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

	struct _PALETTE {
		Uint8 r;
		Uint8 g;
		Uint8 b;
		Uint8 a;
	};
	std::vector<_PALETTE> pallette;


	SDL_Texture* _glyph_texture = nullptr;
};

#endif // __GFXGLYPH_H__
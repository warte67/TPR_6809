// * GfxTile16.h ***************************************
// *
// *  16x16 Tile Mode
// ************************************
#pragma once
#ifndef __GFXTILE_H__
#define __GFXTILE_H__

class GfxTile16 : public GfxMode
{
	friend class GfxTile32;
public:
	GfxTile16();
	virtual ~GfxTile16() {}

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;

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

	SDL_Texture* _tile_texture = nullptr;
	std::vector<GFX::PALETTE> default_palette;
	int pixel_width;
	int pixel_height;
};

#endif // __GFXTILE_H__
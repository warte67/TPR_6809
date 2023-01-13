// * GfxDebug.h ***************************************
// *
// *  The Debugger
// ************************************
#pragma once
#ifndef __GFXDEBUG_H__
#define __GFXDEBUG_H__

class GfxMode;

class GfxDebug : public GfxMode
{
public:
	GfxDebug();
	virtual ~GfxDebug();

	Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);

	virtual void OnInitialize() override;
	virtual void OnQuit() override;
	virtual void OnEvent(SDL_Event* evnt) override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float fElapsedTime) override;
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnRender() override;

private:
	SDL_Texture* debug_texture = nullptr;	// debug screen texture
	std::vector<SDL_Texture*> glyph_textures;

};


#endif //__GFXDEBUG_H__

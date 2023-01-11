// * GfxMode.h ***************************************
// *
// *  Base graphics Mode.  
// ************************************
#pragma once
#ifndef __GFXMODE_H__
#define __GFXMODE_H__

class GFX;

class GfxMode
{
public:
	GfxMode();
	~GfxMode();

	virtual void OnInitialize() {};					// runs once after all devices are created
	virtual void OnQuit() {};						// fires on exit -- reverses OnInitialize()
	virtual void OnEvent(SDL_Event* evnt) {};		// fires per SDL_Event
	// TODO: Make these three abstract
	virtual void OnCreate() {};						// fires when the object is created/recreated
	virtual void OnDestroy() {};					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) {};	// fires each frame, for updates

	GFX* gfx = nullptr;
	Bus* bus = nullptr;
private:
	
};

class GfxNull : public GfxMode
{
public:
	GfxNull();
	virtual void OnUpdate(float fElapsedTime) override;	// fires each frame, for updates

private:
};

#endif //__GFXMODE_H__


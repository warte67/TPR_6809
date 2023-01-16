// * GfxMode.h ***************************************
// *
// *  Base graphics Mode.  
// ************************************
#pragma once
#ifndef __GFXMODE_H__
#define __GFXMODE_H__

#include "types.h"

class GFX;
class Bus;

class GfxMode
{
public:
	GfxMode();
	virtual ~GfxMode();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead) { return 0xCC; }

	virtual void OnInitialize() {}					// runs once after all devices are created
	virtual void OnQuit() {}						// fires on exit -- reverses OnInitialize()
	virtual void OnEvent(SDL_Event* evnt) {}		// fires per SDL_Event
	virtual void OnCreate() {}						// fires when the object is created/recreated
	virtual void OnDestroy() {}						// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime);		// fires each frame, for updates
	virtual void OnActivate() {}
	virtual void OnDeactivate() {}
	virtual void OnRender() {}

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


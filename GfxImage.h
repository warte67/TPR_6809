/**** GfxImage.h ***************************************
 *
 *  Emulates the external 64k Static Ram Chip that is used for extra video ram.
 *  Handles the SDL Images for the GfxIndexed, GfxTile16, GfxTile32, and GfxSprite modes.
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#ifndef __GFXIMAGE_H__
#define __GFXIMAGE_H__
#pragma once





class GfxImage : public GfxMode
{
	friend class GFX;

public:
	GfxImage();
	virtual ~GfxImage();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead) { return 0xCC; }
	virtual Word MapDevice(MemoryMap* memmap, Word offset) { return offset; };

	virtual void OnInitialize();					// runs once after all devices are created
	virtual void OnQuit();							// fires on exit -- reverses OnInitialize()

	virtual void OnCreate();						// fires when the object is created/recreated
	virtual void OnDestroy();						// fires when the object is destroyed/lost focus

	virtual void OnActivate();
	virtual void OnDeactivate();

	virtual void OnEvent(SDL_Event* evnt);		// fires per SDL_Event
	virtual void OnUpdate(float fElapsedTime);	// fires each frame, for updates
	virtual void OnRender();

	static std::array<Byte, 65536> s_mem_64k;
	static Word s_mem_64k_adr;

private:
	GFX* gfx = nullptr;
	Bus* bus = nullptr;

};


#endif // __GFXIMAGE_H__


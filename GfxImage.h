/**** GfxImage.h ***************************************
 *
 *  Emulates the external 64k QSPI Static Ram Chip that is used for external video ram.
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

	std::array<SDL_Texture*, 256> m_images	{ nullptr };
};


#endif // __GFXIMAGES_H__



/**** NOTES: *********************************************************

	s_mem_64k[65536]	internal representation of the 64k QSPI static ram chip.
	s_mem_64k_adr		internal pointer within the 64k static ram block

	m_images[256]		SDL_Texture* the array of 256 SDL Sprites and Tiles.


	Reserved Image Blocks:
		Images 0-191 act as the image buffer for the indexed display mode, but are
		otherwise available when the indexed display mode is not active. The Indexed
		display mode should reserve the first 192 image blocks for its own use. 		
		
		Contiguous image blocks should be able to be used as indices for the tiled 
		display modes. Perhaps by reserving a number of blocks to use as the tilemap 
		memory. This would mean that the tile display modes should be configurable 
		to use either the tile memory or standard mapped memory.



	

**********************************************************************/


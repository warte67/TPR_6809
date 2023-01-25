// * GfxIndexed.h ***************************************
// *
// * 128x80 x 256-Color Indexed (10KB) - Serial Buffer
// ************************************
#ifndef __GFXINDEXED_H__
#define __GFXINDEXED_H__
#pragma once



#include "GfxMode.h"

class GfxIndexed : public GfxMode
{
public:
	GfxIndexed();
	virtual ~GfxIndexed();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;

	virtual void OnInitialize() override;
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float fElapsedTime) override;
	virtual void OnRender() override;

private:
	void cmd_scroll_left();
	void cmd_scroll_right();
	void cmd_scroll_up();
	void cmd_scroll_down();

private:

	SDL_Texture* bitmap_texture = nullptr;
	const int pixel_width = 128;
	const int pixel_height = 80;

	std::vector<GFX::PALETTE> palette256;
	static Uint8 m_palette_index;
	Uint8 red(Uint8 index) { Uint8 c = palette256[index].r;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 grn(Uint8 index) { Uint8 c = palette256[index].g;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 blu(Uint8 index) { Uint8 c = palette256[index].b;  return c | (c << 4) | (c << 8) | (c << 12); }
	Uint8 alf(Uint8 index) { Uint8 c = palette256[index].a;  return c | (c << 4) | (c << 8) | (c << 12); }

	Byte _arg1 = 0xCC;		// default for non-initialized

	bool _bUsingFirstPage = true;
	Word _buffer_base = 0;

};

#endif // __GFXINDEXED_H__


/**** NOTES: *****************************************

There are two indexed buffer pages that should be swapped manually.

Already Implemented:
	GFX_EXT_ADDR		// (Word) 20K extended graphics addresses (doesnt consider buffer page)
	GFX_EXT_DATA		// (Byte) 20K extended graphics RAM data  (doesnt consider buffer page)

Future Implementation:
	GFX_BG_ADDR		// (Word) 10K active graphics address (0 based)
	GFX_BG_DATA		// (Byte) 10K active graphics RAM data

	GFX_BG_ARG1			// (Byte) first read/write argument for the blit commands

	GFX_BG_CMD			// (Byte) Commands to be issued on the 20k indexed bitmap buffer
		// 0x01: Clear Screen       (with color index in GFX_BG_ARG1)
		// 0x02: Set Active Page    (0 or 1 in GFX_BG_ARG1)
		// 0x03: Swap Pages         (simply swaps video buffers)
		// 0x04: Scroll Left		(by pixels x GFX_BG_ARG1)
		// 0x05: Scroll Right       (by pixels x GFX_BG_ARG1)
		// 0x06: Scroll Up          (by pixels x GFX_BG_ARG1)
		// 0x07: Scroll Down        (by pixels x GFX_BG_ARG1)
					
		potential future commands:

		// 0x05 blit from ram to v20_buffer (need x, y, width, height, and buffer address)
		// 0x06 blit from v20_buffer to ram (need x, y, width, height, and buffer address)

	256-Color Palette Specific to the Indexed and Tiled Modes, implement in the base-GFX object:

	// (GFX)  change GFX_PAL_INDX and GFX_PAL_DATA to GFX_FG_PAL_IDX and GFX_FG_PAL_DAT
	// (GFX)  add GFX_BG_PAL_IDX and GFX_BG_PAL_DAT


*******************************************************/

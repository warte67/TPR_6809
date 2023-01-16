// * GfxTile32.h ***************************************
// *
// *  16x16 Overscan Tile Mode
// ************************************
#pragma once
#ifndef __GFXTILE32_H__
#define __GFXTILE32_H__


class GfxTile32 : public GfxTile16
{
public:
	GfxTile32();
	virtual ~GfxTile32() {}

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)  override;
};

#endif // __GFXTILE32_H__
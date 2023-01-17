// * GfxTile32.h ***************************************
// *
// *  16x16 Tile Mode
// * 
// *   This could be a very simple override of GfxTile16() rather
// *		than a whole new class.
// ************************************

#include "types.h"
#include "C6809.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxTile16.h"
#include "GfxTile32.h"


// Graphics Mode Unique Callback Function:
Byte GfxTile32::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		//printf("GfxTile32::OnCallback() -- READ\n");
	}
	else
	{	// WRITE
		//printf("GfxTile32::OnCallback() -- WRITE\n");
	}
	return data;
}

GfxTile32::GfxTile32()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;

	pixel_width = 256;
	pixel_height = 160;
}

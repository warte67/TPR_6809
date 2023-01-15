// * GfxBmp16.cpp ***************************************
// *
// *  128x80 x 16-Color BMP Graphics Mode 
// ************************************

#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxBmp16.h"

// statics:
const int GfxBmp16::pixel_width = 128;
const int GfxBmp16::pixel_height = 80;

// constructor
GfxBmp16::GfxBmp16() : GfxMode()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

// destructor
GfxBmp16::~GfxBmp16()
{
}

void GfxBmp16::OnInitialize()
{
	//printf("GfxGlyph::OnInitialize()\n");
	// 
   // load the default palette
	if (default_palette.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
			{ 0x03 },	// 00 00.00 11		0
			{ 0x07 },	// 00 00.01 11		1
			{ 0x13 },	// 00 01.00 11		2
			{ 0x17 },	// 00 01.01 11		3
			{ 0x83 },	// 01 00.00 11		4
			{ 0x87 },	// 01 00.01 11		5
			{ 0x53 },	// 01 01.00 11		6
			{ 0xCB },	// 10 10.10 11		7
			{ 0x57 },	// 01 01.01 11		8
			{ 0x0F },	// 00 00.11 11		9
			{ 0x33 },	// 00 11.00 11		a
			{ 0x3F },	// 00 11.11 11		b
			{ 0xC3 },	// 11 00.00 11		c
			{ 0xC7 },	// 11 00.11 11		d
			{ 0xF3 },	// 11 11.00 11		e
			{ 0xFF },	// 11 11.11 11		f
		};
		for (int t = 0; t < 16; t++)
			default_palette.push_back(ref[t]);
	}
}

void GfxBmp16::OnActivate()
{
	// load the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxBmp16::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 16; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, gfx->palette[t].color);
	}
}


void GfxBmp16::OnCreate() 
{
	if (bitmap_texture == nullptr)
	{
		bitmap_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pixel_width, pixel_height);
		SDL_SetTextureBlendMode(bitmap_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);
	}
}

void GfxBmp16::OnDestroy() 
{
	if (bitmap_texture)
	{
		SDL_DestroyTexture(bitmap_texture);
		bitmap_texture = nullptr;
	}
}

void GfxBmp16::OnUpdate(float fElapsedTime) 
{
	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);

		Word ofs = VIDEO_START;
		for (int y = 0; y < pixel_height; y++)
		{
			for (int x = 0; x < pixel_width; x += 2)
			{
				Byte data = bus->debug_read(ofs++);
				Byte c1 = data >> 4;
				Byte c2 = data & 0x0f;
				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c1), gfx->grn(c1), gfx->blu(c1), SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);
				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c2), gfx->grn(c2), gfx->blu(c2), SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x+1, y);
			}
		}
	}
}

void GfxBmp16::OnRender() 
{
	SDL_SetRenderTarget(gfx->Renderer(), NULL);
	if (gfx->Fullscreen())
	{
		int ww, wh;
		SDL_GetWindowSize(gfx->Window(), &ww, &wh);
		float fh = (float)wh;
		float fw = fh * gfx->Aspect();
		if (fw > ww)
		{
			fw = (float)ww;
			fh = fw / gfx->Aspect();
		}
		SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
		SDL_RenderCopy(gfx->Renderer(), bitmap_texture, NULL, &dest);
	}
	else
		SDL_RenderCopy(gfx->Renderer(), bitmap_texture, NULL, NULL);
}

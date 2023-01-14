// * GfxBmp4.cpp ***************************************
// *
// *  128x160 x 4-Color BMP Graphics Mode 
// ************************************

#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxBmp4.h"

// statics:
const int GfxBmp4::pixel_width = 128;
const int GfxBmp4::pixel_height = 160;

// constructor
GfxBmp4::GfxBmp4() : GfxMode()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

// destructor
GfxBmp4::~GfxBmp4()
{
}

void GfxBmp4::OnInitialize()
{
	//printf("GfxBmp4::OnInitialize()\n");
	// 
   // load the default palette
	if (default_palette.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
		{ 0x000F },	// 0000 0000 0000 1111		0
		{ 0x0F0F },	// 0000 1111 0000 1111		1
		{ 0xF0FF },	// 1111 0000 1111 1111		2
		{ 0xFF0F },	// 1111 1111 0000 1111		3
		};
		for (int t = 0; t < 4; t++)
			default_palette.push_back(ref[t]);
	}
}

void GfxBmp4::OnActivate()
{
	// load the palette from the defaults
	for (int t = 0; t < 4; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxBmp4::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 4; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write_word(GFX_PAL_DATA, gfx->palette[t].color);
	}
}


void GfxBmp4::OnCreate()
{
	if (bitmap_texture == nullptr)
	{
		bitmap_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pixel_width, pixel_height);
		SDL_SetTextureBlendMode(bitmap_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);
	}
}

void GfxBmp4::OnDestroy()
{
	if (bitmap_texture)
	{
		SDL_DestroyTexture(bitmap_texture);
		bitmap_texture = nullptr;
	}
}

void GfxBmp4::OnUpdate(float fElapsedTime)
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
			for (int x = 0; x < pixel_width; x += 4)
			{
				Byte data = bus->debug_read(ofs++);
				Byte c1 = (data >> 6) & 0x03;
				Byte c2 = (data >> 4) & 0x03;
				Byte c3 = (data >> 2) & 0x03;
				Byte c4 = (data >> 0) & 0x03;

				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c1), gfx->grn(c1), gfx->blu(c1), SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);

				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c2), gfx->grn(c2), gfx->blu(c2), SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x + 1, y);

				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c3), gfx->grn(c3), gfx->blu(c3), SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x + 2, y);

				SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c4), gfx->grn(c4), gfx->blu(c4), SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(gfx->Renderer(), x + 3, y);
			}
		}
	}
}

void GfxBmp4::OnRender()
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

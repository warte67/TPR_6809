// * GfxBmp2.h ***************************************
// *
// *  256x160 x 2-Color BMP Graphics Mode 
// ************************************

#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxBmp2.h"

// statics:
const int GfxBmp2::pixel_width = 256;
const int GfxBmp2::pixel_height = 160;

// constructor
GfxBmp2::GfxBmp2() : GfxMode()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

// destructor
GfxBmp2::~GfxBmp2()
{
}

void GfxBmp2::OnInitialize()
{
	//printf("GfxBmp4::OnInitialize()\n");
	// 
   // load the default palette
	if (default_palette.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
			{ 0x03 },	// 00 00.00 11		0
			{ 0xFF },	// 11 11.11 11		1
		};
		for (int t = 0; t < 2; t++)
		{
			default_palette.push_back(ref[t]);
			bus->write(GFX_PAL_INDX, t);
			bus->write(GFX_PAL_DATA, default_palette[t].color);
		}
	}
}


void GfxBmp2::OnActivate()
{
	// load the palette from the defaults
	for (int t = 0; t < 2; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, default_palette[t].color);
	}
}
void GfxBmp2::OnDeactivate()
{
	// store the palette from the defaults
	for (int t = 0; t < 2; t++)
	{
		bus->write(GFX_PAL_INDX, t);
		bus->write(GFX_PAL_DATA, gfx->palette[t].color);
	}
}




void GfxBmp2::OnCreate()
{
	if (bitmap_texture == nullptr)
	{
		bitmap_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pixel_width, pixel_height);
		SDL_SetTextureBlendMode(bitmap_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);
	}
}

void GfxBmp2::OnDestroy()
{
	if (bitmap_texture)
	{
		SDL_DestroyTexture(bitmap_texture);
		bitmap_texture = nullptr;
	}
}

void GfxBmp2::OnUpdate(float fElapsedTime)
{
	if ((bus->read(GFX_FLAGS) & 0x0f) != 0x00)
		return;

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
			for (int x = 0; x < pixel_width; x += 8)
			{
				Byte data = bus->debug_read(ofs++);
				for (int b = 0; b < 8; b++)
				{					
					Byte c1 = (data >> (7-b)) & 0x01;
					SDL_SetRenderDrawColor(gfx->Renderer(), gfx->red(c1), gfx->grn(c1), gfx->blu(c1), SDL_ALPHA_OPAQUE);
					SDL_RenderDrawPoint(gfx->Renderer(), x + b, y);
				}
			}
		}
	}
}

void GfxBmp2::OnRender()
{
	if ((bus->read(GFX_FLAGS) & 0x0f) != 0x00)
		return;

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


// * GfxRaw.cpp ***************************************
// *
// * 256x160 x 64-Colors
// *		Requires a 40KB Buffer
// ************************************


#include "types.h"
#include "bus.h"
#include "GFX.h"
#include "GfxIndexed.h"


// Graphics Mode Unique Callback Function:
Byte GfxIndexed::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	if (bWasRead)
	{	// READ	
		//printf("GfxRaw::OnCallback() -- READ\n");
	}
	else
	{	// WRITE
		//printf("GfxRaw::OnCallback() -- WRITE\n");
	}
	return data;
}

// constructor
GfxIndexed::GfxIndexed() : GfxMode()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

// destructor
GfxIndexed::~GfxIndexed()
{
}

void GfxIndexed::OnInitialize()
{
	if (palette256.size() == 0)
	{
		std::vector<GFX::PALETTE> ref = {
			{ 0x0000 },	// 0000 0000.0000 1111		0
			{ 0x005F },	// 0000 0000.0101 1111		1
			{ 0x050F },	// 0000 0101.0000 1111		2
			{ 0x055F },	// 0000 0101.0101 1111		3
			{ 0x500F },	// 0101 0000.0000 1111		4
			{ 0x505F },	// 0101 0000.0101 1111		5
			{ 0x550F },	// 0101 0101.0000 1111		6
			{ 0xAAAF },	// 1010 1010.1010 1111		7
			{ 0x555F },	// 0101 0101.0101 1111		8
			{ 0x00FF },	// 0000 0000.1111 1111		9
			{ 0x0F0F },	// 0000 1111.0000 1111		a
			{ 0x0FFF },	// 0000 1111.1111 1111		b
			{ 0xF00F },	// 1111 0000.0000 1111		c
			{ 0xF0FF },	// 1111 0000.1111 1111		d
			{ 0xFF0F },	// 1111 1111.0000 1111		e
			{ 0xFFFF },	// 1111 1111.1111 1111		f
		};
		for (int t = 0; t < 16; t++)
			palette256.push_back(ref[t]);
		// fill out the remaining entries with random junk for now
		Word color = 0x0010;
		for (int t = 16; t < 256; t++)
		{
			GFX::PALETTE ent;
			color += 0x2340;	// rand() % 0x10000;
			ent.color = color;
			palette256.push_back(ent);
		}

	}
}


void GfxIndexed::OnActivate()
{
	//bus->write_word(GFX_EXT_ADDR, 0);
}

void GfxIndexed::OnDeactivate()
{

}




void GfxIndexed::OnCreate()
{
	if (bitmap_texture == nullptr)
	{
		bitmap_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pixel_width, pixel_height);
		SDL_SetTextureBlendMode(bitmap_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);

		// TESTING: ********************************
			//Byte d = 0;
			//for (int t = 0; t < 0x2800; t++)
			//{
			//	bus->write_word(GFX_EXT_ADDR, t);
			//	if (d == 0xff)	d = 0;
			//	bus->write(GFX_EXT_DATA, d++);
			//}
		// TESTING: ********************************
	}
}

void GfxIndexed::OnDestroy()
{
	if (bitmap_texture)
	{
		SDL_DestroyTexture(bitmap_texture);
		bitmap_texture = nullptr;
	}
}

void GfxIndexed::OnUpdate(float fElapsedTime)
{
	// only update once every 10ms (timing my need further adjustment)
	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), bitmap_texture);


		// TESTING: ********************************
			//for (int t = 0; t < 0x5000; t++)
			//{
			//	bus->write_word(GFX_EXT_ADDR, t);
			//	Byte data = bus->read(GFX_EXT_DATA);
			//	data++;
			//	bus->write(GFX_EXT_DATA, data);
			//}
		// TESTING: ********************************


		Word addr = 0;
		for (int y = 0; y < pixel_height; y++)
		{
			for (int x = 0; x < pixel_width; x++)
			{
				//bus->write_word(GFX_EXT_ADDR, addr++);
				//Byte data = bus->read(GFX_EXT_DATA);
				Byte data = GfxMode::s_mem_64k[addr++];
				Byte r = red(data);
				Byte g = grn(data);
				Byte b = blu(data);
				Byte a = SDL_ALPHA_OPAQUE;	// alf(data);
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, a);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);
			}
		}
	}
}

void GfxIndexed::OnRender()
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


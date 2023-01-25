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
		//printf("GfxIndexed::OnCallback() -- READ\n");

		if (ofs == GFX_BG_ARG1)		
			data = _arg1;
	}
	else
	{	// WRITE
		//printf("GfxIndexed::OnCallback() -- WRITE\n");

		if (ofs == GFX_BG_ARG1)		_arg1 = data;

		if (ofs == GFX_BG_CMD)
		{
			switch (data)
			{
				case 0x00:		// undefined or error
					break;

				case 0x01:		// Clear Screen       (with color index in GFX_BG_ARG1)
					for ( int a = _buffer_base; a < _buffer_base + 10240; a++)
						s_mem_64k[a] = _arg1;
					break;

				case 0x02:		// Set Active Page  (zero or non-zero in GFX_BG_ARG1)
					_bUsingFirstPage = (data != 0);
					break;

				case 0x03:		// Swap Pages or flip (simply swaps active video buffers)
					_bUsingFirstPage = !_bUsingFirstPage;
					(_bUsingFirstPage) ? _buffer_base : _buffer_base = 10240;
					break;

				case 0x04:		// Scroll Left      (by pixels x GFX_BG_ARG1)
					cmd_scroll_left();
					break;

				case 0x05:		// Scroll Right     (by pixels x GFX_BG_ARG1)
					cmd_scroll_right();
					break;

				case 0x06:		// Scroll Up        (by pixels x GFX_BG_ARG1)
					cmd_scroll_up();
					break;

				case 0x07:		// Scroll Down      (by pixels x GFX_BG_ARG1)
					cmd_scroll_down();
					break;

				default:		// default (write a 0 to indicate an error)
					data = 0;
					break;
			}
		}
		// write to memory
		bus->debug_write(ofs, data);
	}
	return data;
}

void GfxIndexed::cmd_scroll_left()
{
	if (_arg1 > 16)	_arg1 = 16;
	for (int t = 0; t < _arg1; t++)		// brute force method? optimize later
	{
		for (int y = 0; y < pixel_height; y++)
		{
			// fetch the first pixel
			Byte f_pix = s_mem_64k[_buffer_base + y * pixel_width];
			// scroll the line
			for (int x = 1; x < pixel_width; x++)
			{
				Word a = _buffer_base + (y * pixel_width) + x;				
				s_mem_64k[a - 1] = s_mem_64k[a];
			}
			// set the last pixel
			s_mem_64k[y * pixel_width + (pixel_width - 1)] = f_pix;
		}
	}
}

void GfxIndexed::cmd_scroll_right()
{
	if (_arg1 > 16)	_arg1 = 16;
	for (int t = 0; t < _arg1; t++)		// brute force method? optimize later
	{
		for (int y = 0; y < pixel_height; y++)
		{
			// fetch the first pixel
			Byte f_pix = s_mem_64k[_buffer_base + y * pixel_width + (pixel_width-1)];
			// scroll the line
			for (int x = pixel_width - 2; x >= 0; x--)
			{
				Word a = _buffer_base + (y * pixel_width) + x;
				s_mem_64k[a + 1] = s_mem_64k[a];
			}
			// set the last pixel
			s_mem_64k[y * pixel_width] = f_pix;
		}
	}
}
void GfxIndexed::cmd_scroll_up()
{
	if (_arg1 > 16)	_arg1 = 16;
	for (int t = 0; t < _arg1; t++)		// brute force method? optimize later
	{
		for (int x = 0; x < pixel_width; x++)
		{
			// fetch the first pixel
			Byte f_pix = s_mem_64k[_buffer_base + x];
			// scroll the line
			for (int y = 1; y < pixel_height; y++)
			{
				Word adr1 = _buffer_base + ((y-1) * pixel_width) + x;
				Word adr2 = _buffer_base + ((y+0) * pixel_width) + x;
				s_mem_64k[adr1] = s_mem_64k[adr2];
			}
			// store the pixel
			Word adr1 = _buffer_base + ((pixel_height - 1) * pixel_width) + x;
			s_mem_64k[adr1] = f_pix;
		}
	}
}
void GfxIndexed::cmd_scroll_down()
{
	if (_arg1 > 16)	_arg1 = 16;
	for (int t = 0; t < _arg1; t++)		// brute force method? optimize later
	{
		for (int x = 0; x < pixel_width; x++)
		{
			// fetch the first pixel
			Word adr1 = _buffer_base + ((pixel_height - 1) * pixel_width) + x;
			Byte f_pix = s_mem_64k[adr1];
			// scroll the line
			for (int y = pixel_height-1; y > 0; y--)
			{
				Word adr1 = _buffer_base + ((y - 1) * pixel_width) + x;
				Word adr2 = _buffer_base + ((y + 0) * pixel_width) + x;
				s_mem_64k[adr2] = s_mem_64k[adr1];
			}
			// store the pixel
			adr1 = _buffer_base + x;
			s_mem_64k[adr1] = f_pix;
		}
	}
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

		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.g = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.g = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.g = t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.g = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.g = 15 - t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.g = 15 - t;
			ent.b = t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = 15 - t;
			ent.g = t;
			ent.b = 15 - t;
			palette256.push_back(ent);
		}
		for (int t = 0; t < 16; t++)
		{
			GFX::PALETTE ent = { 0x000f };
			ent.r = t;
			ent.g = 15 - t;
			ent.b = 15 - t;
			palette256.push_back(ent);
		}

		// fill out the remaining entries with random junk for now
		Word color = 0x0010;
		while (palette256.size() < 256)
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

		Word addr = 0;
		for (int y = 0; y < pixel_height; y++)
		{
			for (int x = 0; x < pixel_width; x++)
			{
				// TODO: read data according to the active page
				// ...		bool _bUsingFirstPage
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


/**** GfxIndexed.cpp ***************************************
 *
 * 256x160 x 256-Colors Indexed Mode (external 40k buffer)
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/


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
					for ( int a = _buffer_base; a < _buffer_base + (pixel_width * pixel_height); a++)
						s_mem_64k[a] = _arg1;
					break;

				case 0x02:		// TODO: need a new command  ...      WAS: Set Active Page  (zero or non-zero in GFX_BG_ARG1)
					//_bUsingFirstPage = (data != 0);
					break;

				case 0x03:		// TODO: need a new command  ...      WAS: Swap Pages or flip (simply swaps active video buffers)
					//_bUsingFirstPage = !_bUsingFirstPage;
					//(_bUsingFirstPage) ? _buffer_base : _buffer_base = (pixel_width * pixel_height);
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

				case 0x08:		// Copy Buffer      (From Active to Inactive)
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

void GfxIndexed::cmd_copy_buffer()
{
	// copy from the active buffer to the inactive one
	Word front = 0;
	Word back = 10240;
	if (!_bUsingFirstPage) { front = 10240;  back = 0; }
	for (int t = 0; t < 10240; t++)
		s_mem_64k[front++] = s_mem_64k[back++];
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
	//printf("GfxIndexed::OnActivate()\n");

	//bus->write_word(GFX_EXT_ADDR, 0);

	// ******  TESTING  **************************

					static int pix = 0;

					std::string image_path;
					switch (pix)
					{
					case 0:
						image_path = "resources/vapor.bmp";
						break;
					case 1:
						image_path = "resources/velvet.bmp";
						break;
					case 2:
						image_path = "resources/CoalD_S.bmp";
						break;
					case 3:
						image_path = "resources/TPR_256x160.bmp";
						break;
					case 4:
						image_path = "resources/abstract.bmp";
						break;
					case 5:
						image_path = "resources/Deposit.bmp";
						break;
					}
					pix++;
					pix %= 6;

					SDL_Surface* image = SDL_LoadBMP(image_path.c_str());
					/* Let the user know if the file failed to load */
					if (!image) {
						printf("Failed to load image at %s: %s\n", image_path.c_str(), SDL_GetError());
						return;
					}
					// copy the image to the s_mem_64k buffer	
					Word addr = 0;
					SDL_LockSurface(image);
					for (int y = 0; y < pixel_height; y++)
					{
						for (int x = 0; x < pixel_width; x++)
						{
							int bpp = image->format->BytesPerPixel;
							Byte* p = (Byte*)image->pixels + (int)y * image->pitch + (int)x * bpp;
							Byte index = *(Uint32*)p;
							s_mem_64k[addr++] = index;
							if (addr >= (pixel_width * pixel_height))		addr = 0;
						}
					}
					SDL_UnlockSurface(image);
					// load the palette

					int ncolors = image->format->palette->ncolors;
					//printf("ncolors: %d\n", ncolors);
					for (int index = 0; index <= ncolors;  index++)
					{
						Byte r = image->format->palette->colors[index].r;
						Byte g = image->format->palette->colors[index].g;
						Byte b = image->format->palette->colors[index].b;
						Byte a = image->format->palette->colors[index].a;

						//printf("R:$%02X  G:$%02X  B:$%02X  A:$%02X\n", r, g, b, a);
						palette256[index].r = r >> 4;
						palette256[index].g = g >> 4;
						palette256[index].b = b >> 4;
						palette256[index].a = a >> 4;
					}


					/* Make sure to eventually release the surface resource */
					SDL_FreeSurface(image);
	// ******  TESTING  **************************


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
				Byte data = GfxMode::s_mem_64k[addr++];
				Byte r = red(data);
				Byte g = grn(data);
				Byte b = blu(data);
				Byte a = SDL_ALPHA_OPAQUE;	// alf(data);
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, a);
				SDL_RenderDrawPoint(gfx->Renderer(), x, y);

				if (addr > (pixel_width * pixel_height))	addr = 0;
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


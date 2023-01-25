/**** GfxMouse.cpp ***************************************
 *
 *  Handles the mouse cursor.
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#include "types.h"
#include "Bus.h"
#include "GfxMode.h"
#include "GfxMouse.h"


// Byte GfxMouse::s_size = 8;		// default mouse cursor size (0-15); 0=off


Byte GfxMouse::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	//printf("GfxMouse::OnCallback()\n");

	if (bWasRead)
	{ // READ	
		switch (ofs)
		{
			case CSR_XPOS:		data = mouse_x >> 8;					break;
			case CSR_XPOS + 1:	data = mouse_x & 0xff;					break;
			case CSR_YPOS:		data = mouse_y >> 8;					break;
			case CSR_YPOS + 1:	data = mouse_y & 0xff;					break;
			case CSR_XOFS:		data = mouse_x_offset;					break;
			case CSR_YOFS:		data = mouse_y_offset;					break;
			case CSR_SIZE:		data = m_size;							break;
			case CSR_SCROLL:	data = mouse_wheel;	mouse_wheel = 0;	break;
			case CSR_FLAGS:		data = button_flags;					break;
			case CSR_PAL_INDX:	data = m_palette_index;					break;

			case CSR_PAL_DATA:	data = (default_palette[m_palette_index].color) >> 8;		break;
			case CSR_PAL_DATA+1:data = default_palette[m_palette_index].color & 0xFF;		break;

			case CSR_BMP_INDX:	data = bmp_offset;						break;
			case CSR_BMP_DATA:
				data = cursor_buffer[bmp_offset / 16][bmp_offset % 16];	
				break;
		}

		bus->debug_write(ofs, data);	// pre-write
		return data;
	}
	else
	{ // WRITE
		// for CSR_XPOS && CSR_YPOS see: SDL_WarpMouseInWindow and SDL_WarpMouseGlobal mouse_x = something;
		switch (ofs)
		{
			// case CSR_XPOS:		mouse_x = something;	break;
			// case CSR_XPOS + 1:	mouse_x = something;	break;
			// case CSR_YPOS:		mouse_x = something;	break;
			// case CSR_YPOS + 1:	mouse_x = something;	break;
			case CSR_XOFS:	mouse_x_offset = data;	break;
			case CSR_YOFS:	mouse_y_offset = data;	break;
			case CSR_SIZE:	
				m_size = data;	
				if (m_size >= 0x20)		
					m_size = 0x20;
				bus->debug_write(CSR_SIZE, m_size); 
				return m_size;
				break;
			case CSR_SCROLL: mouse_wheel = data;	break;
			case CSR_FLAGS:	return data;			break;		// read only
			case CSR_PAL_INDX:	
				m_palette_index = data;		
				bus->debug_write(CSR_PAL_DATA, default_palette[m_palette_index].color);
				break;

			case CSR_PAL_DATA:
				default_palette[m_palette_index].color = 
					(default_palette[m_palette_index].color & 0x00FF) | (data << 8);
				bIsDirty = true;
				break;

			case CSR_PAL_DATA+1:
				default_palette[m_palette_index].color =
					(default_palette[m_palette_index].color & 0xFF00) | (data << 0);
				bIsDirty = true;
				break;

			case CSR_BMP_INDX:
				bmp_offset = data;	
				bus->debug_write_word(CSR_BMP_DATA, cursor_buffer[bmp_offset / 16][bmp_offset % 16]);
				break;
			case CSR_BMP_DATA:
				cursor_buffer[bmp_offset / 16][bmp_offset % 16] = data;
				bIsDirty = true;
				break;
		}
		bus->debug_write(ofs, data);
	}

	return data;
}

Word GfxMouse::MapDevice(MemoryMap* memmap, Word offset)
{
	std::string reg_name = "Debug System";
	DWord st_offset = offset;

	// map fundamental mouse cursor hardware registers:
	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Mouse Cursor Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "CSR_BEGIN", "start of mouse cursor hardware registers" }); offset += 0;
	memmap->push({ offset, "CSR_XPOS", "(Word) horizontal mouse cursor coordinate" }); offset += 2;
	memmap->push({ offset, "CSR_YPOS", "(Word) vertical mouse cursor coordinate" }); offset += 2;
	memmap->push({ offset, "CSR_XOFS", "(Byte) horizontal mouse cursor offset" }); offset += 1;
	memmap->push({ offset, "CSR_YOFS", "(Byte) vertical mouse cursor offset" }); offset += 1;
	memmap->push({ offset, "CSR_SIZE", "(Byte) cursor size (0-15) 0:off" }); offset += 1;
	memmap->push({ offset, "CSR_SCROLL", "(Signed) MouseWheel Scroll: -1, 0, 1" }); offset += 1;
	memmap->push({ offset, "CSR_FLAGS", "(Byte) mouse button flags:" }); offset += 1;
	memmap->push({ offset, "", ">    bits 0-5: button states" }); offset += 0;
	memmap->push({ offset, "", ">    bits 6-7: number of clicks" }); offset += 0;
	memmap->push({ offset, "CSR_PAL_INDX", "(Byte) mouse cursor color palette index (0-15)" }); offset += 1;
	memmap->push({ offset, "CSR_PAL_DATA", "(Word) mouse cursor color palette data RGBA4444" }); offset += 2;
	memmap->push({ offset, "CSR_BMP_INDX", "(Byte) mouse cursor bitmap pixel offset" }); offset += 1;
	memmap->push({ offset, "CSR_BMP_DATA", "(Byte) mouse cursor bitmap pixel index color" }); offset += 1;
	memmap->push({ --offset, "CSR_END", "end of mouse cursor hardware registers" }); offset += 1;

	return offset - st_offset;
}


GfxMouse::GfxMouse()
{
	//printf("GfxMouse::GfxMouse()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;

	// Initialaize the palette
	if (default_palette.size() == 0)
	{
		//for (int t = 0; t < 16; t++)
		//	default_palette.push_back({ 0x00 });
		std::vector<GFX::PALETTE> ref = {
			{ 0x0000 },		// 0000 0000.0000 0000	0
			{ 0x0005 },		// 0000 0000.0000 0101	1
			{ 0x000A },		// 0000 0000.0000 1010	2
			{ 0x000F },		// 0000 0000.0000 1111	3
			{ 0xFFFF },		// 1111 1111.1111 1111	4
			{ 0xCCCF },		// 1010 1010.1010 1111	5
			{ 0x555F },		// 0101 0101.0101 1111	6
			{ 0x00CF },		// 0000 0000.1010 1111	7
			{ 0x0C0F },		// 0000 1010.0000 1111	8
			{ 0xC00F },		// 1010 0000.0000 1111	9
			{ 0x00FF },		// 0000 0000.1111 1111	a
			{ 0x0F0F },		// 0000 1111.0000 1111	b
			{ 0x0FFF },		// 0000 1111.1111 1111	c
			{ 0xF0FF },		// 1111 0000.1111 1111	d
			{ 0xFF0F },		// 1111 1111.0000 1111	e
			{ 0xFFFF },		// 1111 1111.1111 1111	f
		};
		for (int t = 0; t < 16; t++)
			default_palette.push_back(ref[t]);
	}
}

GfxMouse::~GfxMouse()
{
}


void GfxMouse::OnInitialize()
{
	//printf("GfxMouse::OnInitialize() \n");

	// prepare mems
	bus->debug_write(CSR_SIZE, m_size);
}

void GfxMouse::OnActivate() 
{
	//// load the palette from the defaults
	//for (int t = 0; t < 16; t++)
	//{
	//	bus->write(GFX_PAL_INDX, t);
	//	bus->write_word(GFX_PAL_DATA, default_palette[t].color);
	//}
}

void GfxMouse::OnDeactivate() 
{
	//// store the palette from the defaults
	//for (int t = 0; t < 16; t++)
	//{
	//	bus->write(GFX_PAL_INDX, t);
	//	bus->write_word(GFX_PAL_DATA, gfx->palette[t].color);
	//}
}


void GfxMouse::OnQuit()
{
	//printf("GfxMouse::OnQuit() \n");
}

void GfxMouse::OnEvent(SDL_Event* evnt)
{
	int display_num = bus->read(GFX_AUX) & 0x07;
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(display_num, &dm);
	int ww = gfx->WindowHeight();
	int wh = gfx->WindowHeight();
	SDL_Rect dest = { dm.w / 2 - ww / 2, dm.h / 2 - wh / 2, ww, wh };

	switch (evnt->type)
	{
		case SDL_MOUSEMOTION:
		{
			SDL_Surface* surf = SDL_GetWindowSurface(gfx->Window());
			float sw = (float)surf->w;
			float sh = (float)surf->h;
			float rw = (float)gfx->ResWidth();
			float rh = (float)gfx->ResHeight();
			float w_aspect = sw / rw;
			float h_aspect = sh / rh;
			mouse_x_screen = evnt->button.x;
			mouse_y_screen = evnt->button.y;
			mouse_x = int((float)mouse_x_screen / w_aspect);
			mouse_y = int((float)mouse_y_screen / h_aspect);
			
			if (gfx->Fullscreen())
			{
				 // trim to clipping region
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
				w_aspect = (float)dest.w / rw;
				h_aspect = (float)dest.h / rh;
				int mx = int((mouse_x_screen/w_aspect) - (dest.x/w_aspect));
				if (mx < 0)  mx = 0;
				if (mx >= rw) mx = (int)rw - 1;
				int my = int((mouse_y_screen / h_aspect) - (dest.y / h_aspect));
				if (my < 0)  my = 0;
				if (my >= rh) my = (int)rh - 1;				
				mouse_x = mx;
				mouse_y = my;
			}
			break;
		}

		//		CSR_FLAGS = 0x1811,        // (Byte) mouse button flags:
		//			//      bits 0-5: button states
		//			//      bits 6-7: number of clicks
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			// update the button flags
			button_flags = 0;
			int bitmask = (1 << ((evnt->button.button % 7) - 1));
			if (evnt->button.state == 0)
				button_flags &= ~bitmask;
			else
			{
				button_flags |= bitmask;
				button_flags |= (evnt->button.clicks & 0x03) << 6;
			}
			bus->debug_write(CSR_FLAGS, button_flags);
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			// printf("MOUSEWHEEL: %d\n", evnt->wheel.y);
			mouse_wheel = evnt->wheel.y;
			bus->debug_write(CSR_SCROLL, mouse_wheel);
			break;
		}
	}
}

void GfxMouse::OnCreate()
{
	if (mouse_texture == nullptr)
	{
		mouse_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, 16, 16);
		SDL_SetTextureBlendMode(mouse_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), mouse_texture);
		SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0x00);
		SDL_RenderClear(gfx->Renderer());
		// build the mouse cursor texture
		for (int h = 0; h < 16; h++)
		{
			for (int v = 0; v < 16; v++)
			{
				Byte i = cursor_buffer[v][h] & 0x0f;
				Byte r = _red(i);
				Byte g = _grn(i);
				Byte b = _blu(i);
				Byte a = _alf(i);
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, a);
				SDL_RenderDrawPoint(gfx->Renderer(), h, v);
			}
		}
		SDL_SetRenderTarget(gfx->Renderer(), NULL);
	}

	// set up clipping
	if (gfx->Fullscreen())
	{
		// fetch the actual current display resolution
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
		SDL_RenderSetClipRect(gfx->Renderer(), &dest);
	}
	else
		SDL_RenderSetClipRect(gfx->Renderer(), NULL);
}

void GfxMouse::OnDestroy()
{
	if (mouse_texture)
	{
		SDL_DestroyTexture(mouse_texture);
		mouse_texture = nullptr;
	}
}


void GfxMouse::OnUpdate(float fElapsedTime) 
{
	// test mouse callback
	// printf("GfxMouse::OnUpdate() --->  XPOS: %d  YPOS: %d\n", bus->read_word(CSR_XPOS), bus->read_word(CSR_YPOS));

	//if (!gfx->MouseEnabled())
	//	return;

	if (m_size && bIsDirty)
	{
		// update the mouse cursor colors
		SDL_SetRenderTarget(gfx->Renderer(), mouse_texture);
		//SDL_SetTextureBlendMode(mouse_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0x00);
		SDL_RenderClear(gfx->Renderer());
		for (int h = 0; h < 16; h++)
		{
			for (int v = 0; v < 16; v++)
			{
				Byte i = cursor_buffer[v][h] & 0x0f;
				Byte r = _red(i);
				Byte g = _grn(i);
				Byte b = _blu(i);
				Byte a = _alf(i);
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, a);
				SDL_RenderDrawPoint(gfx->Renderer(), h, v);
			}
		}
		bIsDirty = false;
	}
}


void GfxMouse::OnRender()
{
	// render the textures	
	//SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_SetRenderTarget(gfx->Renderer(), NULL);

	// render the texture
	SDL_Rect dst = { mouse_x_screen, mouse_y_screen, m_size * 8, m_size * 8 };
	if (gfx->Fullscreen())
	{
		SDL_Rect clip;
		SDL_RenderGetClipRect(gfx->Renderer(), &clip);
		if (dst.x < clip.x)	dst.x = clip.x;
		if (dst.x > clip.x + clip.w) dst.x = clip.x + clip.w;
		if (dst.y < clip.y)	dst.y = clip.y;
		if (dst.y > clip.y + clip.h) dst.y = clip.y + clip.h;
	}
	dst.x -= mouse_x_offset;
	dst.y -= mouse_y_offset;

	SDL_RenderCopy(gfx->Renderer(), mouse_texture, NULL, &dst);
}

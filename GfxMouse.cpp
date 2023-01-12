// * GfxMouse.cpp ***************************************
// *
// *  Handles the mouse cursor.
// ************************************

#include "types.h"
#include "Bus.h"
#include "GfxMode.h"
#include "GfxMouse.h"


Byte GfxMouse::s_size = 8;		// default mouse cursor size (0-15); 0=off

//  Mouse Cursor Hardware Registers:
//      CSR_XPOS = 0x1808,        // (Word) horizontal mouse cursor coordinate
//      CSR_YPOS = 0x180a,        // (Word) vertical mouse cursor coordinate
//      CSR_XOFS = 0x180c,        // (Word) horizontal mouse cursor offset
//      CSR_YOFS = 0x180e,        // (Word) vertical mouse cursor offset
//      CSR_SIZE = 0x1810,        // (Byte) cursor size (0-15) 0:off
//     CSR_FLAGS = 0x1811,        // (Byte) mouse button flags:
//                                //      bits 0-5: button states
//                                //      bits 6-7: number of clicks
//  CSR_PAL_INDX = 0x1812,        // (Byte) mouse color palette color index (0-15)
//  CSR_PAL_DATA = 0x1813,        // (Byte) mouse color palette color bits RRGGBBAA

Byte GfxMouse::OnCallback(REG* memDev, Word ofs, Byte data, bool bWasRead)
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
			case CSR_XOFS:		data = mouse_x_offset >> 8;				break;
			case CSR_XOFS + 1:	data = mouse_x_offset & 0xff;			break;
			case CSR_YOFS:		data = mouse_y_offset >> 8;				break;
			case CSR_YOFS + 1:	data = mouse_y_offset & 0xff;			break;
			case CSR_SIZE:		data = s_size;							break;
			case CSR_FLAGS:		data = button_flags;					break;
			case CSR_PAL_INDX:	data = m_palette_index;					break;
			case CSR_PAL_DATA:	data = palette[m_palette_index].color;	break;
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
			case CSR_XOFS:
				mouse_x_offset = (mouse_x_offset & 0x00ff) | (data << 8);	break;
			case CSR_YOFS:
				mouse_x_offset = (mouse_x_offset & 0xff00) | (data << 0);	break;
			case CSR_SIZE:	s_size = data % 16;	break;
			case CSR_FLAGS:	return data;	break;		// read only
			case CSR_PAL_INDX:	m_palette_index = data;		break;
			case CSR_PAL_DATA: 
				palette[m_palette_index].color = data;	break;
		}
		bus->debug_write(ofs, data);
	}

	return data;
}


GfxMouse::GfxMouse()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;

	if (palette.size() == 0)
	{
		for (int t = 0; t < 16; t++)
			palette.push_back({ 0x00 });

		std::vector<PALETTE> ref = {
			{ 0x00 },		// 00 00.00 00	0
			{ 0x01 },		// 00 00.00 01	1
			{ 0x02 },		// 00 00.00 10	2
			{ 0x03 },		// 00 00.00 11	3
			{ 0xff },		// 11 11.11 11	4
			{ 0xab },		// 10 10.10 11	5
			{ 0x57 },		// 01 01.01 11	6
			{ 0x0b },		// 00 00.10 11	7
			{ 0x23 },		// 00 10.00 11	8
			{ 0x83 },		// 10 00.00 11	9
			{ 0x0f },		// 00 00.11 11	a
			{ 0x33 },		// 00 11.00 11	b
			{ 0x3f },		// 00 11.11 11	c
			{ 0xcf },		// 11 00.11 11	d
			{ 0xf3 },		// 11 11.00 11	e
			{ 0xff },		// 11 11.11 11	f
		};

		for (int t = 0; t < 16; t++)
		{
			palette[t].color = ref[t].color;

			//bus->write(GFX_PAL_INDX, t);
			//bus->write(GFX_PAL_DATA, ref[t].color);

			// printf("ref: $%02X, R:%1d, G:%1d, B:%1d, A:%1d\n", ref[t].color, ref[t].r, ref[t].g, ref[t].b, ref[t].a);
		}
	}
}

GfxMouse::~GfxMouse()
{
}


void GfxMouse::OnInitialize()
{
	printf("GfxMouse::OnInitialize() \n");
}

void GfxMouse::OnQuit()
{
	printf("GfxMouse::OnQuit() \n");

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
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			printf("DOWN:  which:%d  button:%d  state:%d  clicks:%d\n",
				evnt->button.which,
				evnt->button.button,
				evnt->button.state,
				evnt->button.clicks );
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			printf("MOUSEWHEEL: %d\n", evnt->wheel.y);
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
				Byte r = red(i);
				Byte g = grn(i);
				Byte b = blu(i);
				Byte a = alf(i);
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

	if (s_size)
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
				Byte r = red(i);
				Byte g = grn(i);
				Byte b = blu(i);
				Byte a = alf(i);
				SDL_SetRenderDrawColor(gfx->Renderer(), r, g, b, a);
				SDL_RenderDrawPoint(gfx->Renderer(), h, v);
			}
		}
	}
}

void GfxMouse::OnActivate() {}
void GfxMouse::OnDeactivate() {}


void GfxMouse::OnRender()
{
	//// set up clipping
	//if (gfx->Fullscreen())
	//{
	//	// fetch the actual current display resolution
	//	int ww, wh;
	//	SDL_GetWindowSize(gfx->Window(), &ww, &wh);
	//	float fh = (float)wh;
	//	float fw = fh * gfx->Aspect();
	//	if (fw > ww)
	//	{
	//		fw = (float)ww;
	//		fh = fw / gfx->Aspect();
	//	}
	//	SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
	//	SDL_RenderSetClipRect(gfx->Renderer(), &dest);	// clip to screen
	//}

	// render the textures	
	SDL_Rect dst = { mouse_x_screen - mouse_x_offset, mouse_y_screen - mouse_y_offset, s_size * 8, s_size * 8 };
	//SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_SetRenderTarget(gfx->Renderer(), NULL);

	// render the texture
	SDL_RenderCopy(gfx->Renderer(), mouse_texture, NULL, &dst);
}

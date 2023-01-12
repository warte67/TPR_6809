// * GfxMouse.cpp ***************************************
// *
// *  Handles the mouse cursor.
// ************************************

#include "types.h"
#include "Bus.h"
#include "GfxMode.h"
#include "GfxMouse.h"


Byte GfxMouse::s_size = 8;		// default mouse cursor size (0-15); 0=off

GfxMouse::GfxMouse()
{
	bus = Bus::getInstance();
	gfx = bus->m_gfx;

	if (palette.size() == 0)
	{
		for (int t = 0; t < 16; t++)
			palette.push_back({ 0,0,0, SDL_ALPHA_OPAQUE });

		std::vector<PALETTE> ref = {
			{ 0x00, 0x00, 0x00,  0x00 },
			{ 0x00, 0x00, 0x00,  0x33 },
			{ 0x00, 0x00, 0x00,  0x66 },
			{ 0x00, 0x00, 0x00,  0xFF },
			{ 0xff, 0xff, 0xff,  0xFF },
			{ 0xdd, 0xdd, 0xdd,  0xFF },
			{ 0xbb, 0xbb, 0xbb,  0xFF },
			{ 0x99, 0x99, 0x99,  0xFF },
			{ 0x77, 0x77, 0x77,  0xFF },
			{ 0x55, 0x55, 0x55,  0xFF },
			{ 0x33, 0x33, 0x33,  0xFF },
			{ 0x55, 0x55, 0x55,  0xFF },
			{ 0x77, 0x77, 0x77,  0xFF },
			{ 0x99, 0x99, 0x99,  0xFF },
			{ 0xbb, 0xbb, 0xbb,  0xFF },
			{ 0xdd, 0xdd, 0xdd,  0xFF },
		};

		for (int t = 0; t < 16; t++)
		{
			//bus->write(GFX_PAL_INDX, t);
			//bus->write(GFX_PAL_RED, ref[t].r);
			//bus->write(GFX_PAL_GRN, ref[t].g);
			//bus->write(GFX_PAL_BLU, ref[t].b);
			//bus->write(GFX_PAL_ALF, ref[t].a);
			palette[t].r = ref[t].r;
			palette[t].g = ref[t].g;
			palette[t].b = ref[t].b;
			palette[t].a = ref[t].a;
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
			float rw = gfx->ResWidth();
			float rh = gfx->ResHeight();

			mouse_x_screen = evnt->button.x;
			mouse_y_screen = evnt->button.y;

			float sx = rw / ww;
			float sy = rh / wh;

			float mx = (float)mouse_x_screen * sx;
			float my = (float)mouse_y_screen * sy;

			mouse_x_pixel = (int)mx;
			mouse_y_pixel = (int)my;

			mouse_x_timing = mouse_x_timing;
			mouse_y_timing = mouse_y_timing;

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
}

void GfxMouse::OnDestroy()
{
	if (mouse_texture)
	{
		SDL_DestroyTexture(mouse_texture);
		mouse_texture = nullptr;
	}
}

void GfxMouse::OnUpdate(float fElapsedTime) {}
void GfxMouse::OnActivate() {}
void GfxMouse::OnDeactivate() {}


void GfxMouse::OnRender()
{
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
		SDL_RenderSetClipRect(gfx->Renderer(), &dest);	// clip to screen
	}

	// render the textures	
	SDL_Rect dst = { mouse_x_screen - mouse_x_offset, mouse_y_screen - mouse_y_offset, s_size * 8, s_size * 8 };
	//SDL_SetRenderTarget(gfx->Renderer(), gfx->Texture());
	SDL_SetRenderTarget(gfx->Renderer(), NULL);

	// render the texture
	SDL_RenderCopy(gfx->Renderer(), mouse_texture, NULL, &dst);
}

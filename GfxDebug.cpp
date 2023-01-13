// * GfxMouse.cpp ***************************************
// *
// *  The Debugger
// ************************************

#include "types.h"
#include "Bus.h"
#include "GfxMode.h"
#include "GfxDebug.h"

//#include "font8x8_system.h"
extern Byte font8x8_system[256][8];

GfxDebug::GfxDebug()
{
	printf("GfxDebug::GfxDebug()\n");
}

GfxDebug::~GfxDebug()
{
	printf("GfxDebug::~GfxDebug()\n");
}


Byte GfxDebug::OnCallback(REG* memDev, Word ofs, Byte data, bool bWasRead)
{
	printf("GfxDebug::OnCallback()\n");

	if (bWasRead)
	{
	}
	else
	{
	}
	return data;
}


void GfxDebug::OnInitialize() 
{ 
	//printf("GfxDebug::OnInitialize()\n"); 
}
void GfxDebug::OnQuit() 
{ 
	//printf("GfxDebug::OnQuit()\n"); 
}

void GfxDebug::OnEvent(SDL_Event* evnt) 
{ 
	//printf("GfxDebug::OnEvent()\n"); 
}

void GfxDebug::OnCreate() 
{ 
	//printf("GfxDebug::OnCreate()\n"); 

	if (debug_texture == nullptr)
	{
		int pw = gfx->PixWidth();
		int ph = gfx->PixHeight();

		debug_texture = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
			SDL_TEXTUREACCESS_TARGET, pw, ph);
		SDL_SetTextureBlendMode(debug_texture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gfx->Renderer(), debug_texture);
	}
	// create the glyph textures
	if (glyph_textures.size() == 0)
	{
		for (int t = 0; t < 256; t++)
		{
			SDL_Texture* glyph = SDL_CreateTexture(gfx->Renderer(), SDL_PIXELFORMAT_RGBA4444,
				SDL_TEXTUREACCESS_TARGET, 8, 8);
			SDL_SetRenderTarget(gfx->Renderer(), glyph);
			SDL_SetTextureBlendMode(glyph, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0);
			SDL_RenderClear(gfx->Renderer());
			for (int y = 0; y < 8; y++)
			{
				for (int x = 0; x < 8; x++)
				{
					Byte bitMask = 1 << (7 - x);
					if (font8x8_system[t][y] & bitMask)
					{
						SDL_SetRenderDrawColor(gfx->Renderer(), 255, 255, 255, 255);
						SDL_RenderDrawPoint(gfx->Renderer(), x, y);
					}
				}
			}
			glyph_textures.push_back(glyph);
		}
	}
}

void GfxDebug::OnDestroy() 
{ 
	//printf("GfxDebug::OnDestroy()\n"); 
	if (debug_texture)
	{
		SDL_DestroyTexture(debug_texture);
		debug_texture = nullptr;
	}
	// destroy the glyph textures
	for (auto& a : glyph_textures)
		SDL_DestroyTexture(a);
	glyph_textures.clear();
}

void GfxDebug::OnUpdate(float fElapsedTime)
{
	//printf("GfxDebug::OnUpdate()\n"); 

	const float delay = 0.010f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), debug_texture);
		SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, 0xAA);
		SDL_RenderClear(gfx->Renderer());

		// TEST:  render a character
		static Uint8 index = 0;
		SDL_Rect dst = { 0, 0, 8, 8 };
		SDL_RenderCopy(gfx->Renderer(), glyph_textures[index++], NULL, &dst);
		SDL_SetRenderTarget(gfx->Renderer(), NULL);

	}
}

void GfxDebug::OnActivate() { printf("GfxDebug::OnActivate()\n"); }
void GfxDebug::OnDeactivate() { printf("GfxDebug::OnDeactivate()\n"); }

void GfxDebug::OnRender() 
{ 
	//printf("GfxDebug::OnRender()\n"); 
	SDL_SetRenderTarget(gfx->Renderer(), NULL);
	//SDL_RenderCopy(gfx->Renderer(), debug_texture, NULL, NULL);

	if (gfx->Fullscreen())		// m_fullscreen
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
		//if (m_enable_backbuffer)
		//	SDL_RenderCopy(_renderer, _texture[1 - m_current_backbuffer], NULL, &dest);
		//else
		//	SDL_RenderCopy(_renderer, _texture[m_current_backbuffer], NULL, &dest);
		SDL_RenderCopy(gfx->Renderer(), debug_texture, NULL, &dest);
	}
	else
	{
		//if (m_enable_backbuffer)
		//	SDL_RenderCopy(_renderer, _texture[1 - m_current_backbuffer], NULL, NULL);
		//else
		//	SDL_RenderCopy(_renderer, _texture[m_current_backbuffer], NULL, NULL);
		SDL_RenderCopy(gfx->Renderer(), debug_texture, NULL, NULL);
	}



}
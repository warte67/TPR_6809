// * GfxMouse.h ***************************************
// *
// *  Handles the mouse cursor.
// ************************************
#pragma once
#ifndef __GFXMOUSE_H__
#define __GFXMOUSE_H__


class GfxMouse : public GfxMode
{
public:
	GfxMouse();
	virtual ~GfxMouse();

	virtual void OnInitialize() override;				
	virtual void OnQuit() override;						
	virtual void OnEvent(SDL_Event* evnt) override;		
	virtual void OnCreate() override;					
	virtual void OnDestroy() override;					
	virtual void OnUpdate(float fElapsedTime) override;	
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnRender() override;

private:

	int mouse_x_screen = 0;
	int mouse_y_screen = 0;
	int mouse_x_pixel = 0;
	int mouse_y_pixel = 0;
	int mouse_x_timing = 0;
	int mouse_y_timing = 0;
	Sint16 mouse_x_offset = 0;		// mouse cursor offset x
	Sint16 mouse_y_offset = 0;		// mouse cursor offset y

	static Byte s_size;
	SDL_Texture* mouse_texture = nullptr;

	Word bmp_offset = 0;
	Byte cursor_buffer[16][16] = {		// indexed 16-color bitmap data for the mouse cursor
		{3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{3,4,3,3,2,1,0,0,0,0,0,0,0,0,0,0},
		{2,3,4,4,3,3,2,1,0,0,0,0,0,0,0,0},
		{1,3,4,4,4,4,3,3,2,1,0,0,0,0,0,0},
		{0,2,3,4,4,4,4,4,3,3,2,1,0,0,0,0},
		{0,1,3,4,4,4,4,4,4,4,3,3,2,1,0,0},
		{0,0,2,3,4,4,4,4,4,4,4,4,3,2,1,0},
		{0,0,1,3,4,4,4,4,4,3,3,3,2,1,0,0},
		{0,0,0,2,3,4,4,4,3,2,1,1,0,0,0,0},
		{0,0,0,1,3,4,4,3,2,1,0,0,0,0,0,0},
		{0,0,0,0,2,3,4,3,1,0,0,0,0,0,0,0},
		{0,0,0,0,1,3,4,3,1,0,0,0,0,0,0,0},
		{0,0,0,0,0,2,3,2,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,1,2,1,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	};

	// Palette Stuff
	struct PALETTE {
		Uint8 r;
		Uint8 g;
		Uint8 b;
		Uint8 a;
	};
	std::vector<PALETTE> palette;
	static Uint8 m_palette_index;
	void red(Uint8 index, Uint8 r) { palette[index].r = r; }
	void grn(Uint8 index, Uint8 g) { palette[index].g = g; }
	void blu(Uint8 index, Uint8 b) { palette[index].b = b; }
	void alf(Uint8 index, Uint8 a) { palette[index].a = a; }

public:
	Uint8 red(Uint8 index) { return palette[index].r; }
	Uint8 grn(Uint8 index) { return palette[index].g; }
	Uint8 blu(Uint8 index) { return palette[index].b; }
	Uint8 alf(Uint8 index) { return palette[index].a; }
};



#endif // __GFXMOUSE_H__
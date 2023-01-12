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

	Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);

	virtual void OnInitialize() override;				
	virtual void OnQuit() override;						
	virtual void OnEvent(SDL_Event* evnt) override;		
	virtual void OnCreate() override;					
	virtual void OnDestroy() override;					
	virtual void OnUpdate(float fElapsedTime) override;	
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;
	virtual void OnRender() override;

	int Mouse_Xpos() { return mouse_x_screen; }
	int Mouse_Ypos() { return mouse_y_screen; }
	int Mouse_Xofs() { return mouse_x_offset; }
	int Mouse_Yofs() { return mouse_y_offset; }


private:

	int mouse_x_screen = 0;
	int mouse_y_screen = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	Sint16 mouse_x_offset = 0;		// mouse cursor offset x
	Sint16 mouse_y_offset = 0;		// mouse cursor offset y
	static Byte s_size;
	Uint8 button_flags = 0;
	SDL_Texture* mouse_texture = nullptr;

	// cursor stuff
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
	union PALETTE {
		Byte color;
		struct {
			Uint8 a : 2;
			Uint8 b : 2;
			Uint8 g : 2;
			Uint8 r : 2;
		};
	};
	std::vector<PALETTE> palette;
	Uint8 m_palette_index;

public:
    Uint8 red(Uint8 index) 
        { Uint8 c = palette[index].r; return c | (c << 2) | (c << 4) | (c << 6); }
    Uint8 grn(Uint8 index) 
        { Uint8 c = palette[index].g; return c | (c << 2) | (c << 4) | (c << 6); }
    Uint8 blu(Uint8 index) 
        { Uint8 c = palette[index].b; return c | (c << 2) | (c << 4) | (c << 6); }
    Uint8 alf(Uint8 index) 
        { Uint8 c = palette[index].a; return c | (c << 2) | (c << 4) | (c << 6); }
};



#endif // __GFXMOUSE_H__
// * GfxDebug.h ***************************************
// *
// *  The Debugger
// ************************************
#pragma once
#ifndef __GFXDEBUG_H__
#define __GFXDEBUG_H__

#include <map>
#include <list>
#include "GfxMode.h"

class GfxMode;

class GfxDebug : public GfxMode
{
	friend class Bus;
	friend class GFX;

public:
	GfxDebug();
	virtual ~GfxDebug();

	virtual Byte OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead) override;
	static Word MapDevice(MemoryMap* memmap, Word offset) ;

	virtual void OnInitialize() override;
	virtual void OnQuit() override;
	virtual void OnEvent(SDL_Event* evnt) override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float fElapsedTime) override;
	virtual void OnRender() override;

	void OutGlyph(int row, int col, Byte glyph, Byte red = 255, Byte grn = 255, Byte blu = 255, bool bDropShadow = true);
	int OutText(int x, int y, std::string text, Byte red, Byte grn, Byte blu, bool bDropshadow = true);
	std::string hex(Uint32 n, Uint8 d);
	void DumpMemory(int col, int row, Word addr);
	void DrawCpu(int x, int y);
	void DrawCode(int col, int row);
	bool EditRegister(float fElapsedTime);
	void DrawCursor(float fElapsedTime);
	bool SingleStep();
	void ContinueSingleStep();
	void MouseStuff();
	bool CoordIsValid(int x, int y);
	void KeyboardStuff();
	void DrawButtons();
	void HandleButtons();

	void SetSingleStep(bool step) { bSingleStep = step; }

	//bool IsMemDumping() { return bDebugMemDump; }

	// button callbacks
	void cbClearBreaks();
	void cbReset();
	void cbNMI();
	void cbIRQ();
	void cbFIRQ();
	void cbRunStop();

protected:
	enum CSR_AT {
		CSR_AT_NONE, CSR_AT_ADDRESS, CSR_AT_DATA, CSR_AT_REGISTER
	};
	enum EDIT_REGISTER {
		EDIT_NONE,
		EDIT_CC, EDIT_D, EDIT_A, EDIT_B, EDIT_X,
		EDIT_Y, EDIT_U, EDIT_PC, EDIT_S, EDIT_DP,
		EDIT_BREAK,
	};
	struct REGISTER_NODE {
		EDIT_REGISTER reg = EDIT_NONE;
		Uint16 value = 0;
		Uint16 y_pos = 0;
		Uint16 x_min = 0;
		Uint16 x_max = 0;
	};
	std::vector<REGISTER_NODE> register_info = {
		{ EDIT_REGISTER::EDIT_NONE, 0, 0, 0, 0 },
		{ EDIT_REGISTER::EDIT_CC, 0, 1, 43, 44 },
		{ EDIT_REGISTER::EDIT_D, 0, 2, 42, 45 },
		{ EDIT_REGISTER::EDIT_A, 0, 2, 51, 52 },
		{ EDIT_REGISTER::EDIT_B, 0, 2, 57, 58 },
		{ EDIT_REGISTER::EDIT_X, 0, 3, 43, 46 },
		{ EDIT_REGISTER::EDIT_Y, 0, 3, 51, 54 },
		{ EDIT_REGISTER::EDIT_U, 0, 3, 59, 62 },
		{ EDIT_REGISTER::EDIT_PC, 0, 4, 43, 46 },
		{ EDIT_REGISTER::EDIT_S, 0, 4, 51, 54 },
		{ EDIT_REGISTER::EDIT_DP, 0, 4, 60, 61 },
		{ EDIT_REGISTER::EDIT_BREAK, 0, 16, 113, 116 },			// TODO: ....
	};
	REGISTER_NODE nRegisterBeingEdited = { EDIT_NONE,0,0,0,0 };

	struct BUTTON_NODE {
		std::string text;		// button text
		SDL_Scancode key;		// shortcut key scancode
		// color attribute
		// ...
		Uint16 x_min;			// button left
		Uint16 x_max;			// button right
		Uint16 y_pos;
		Byte clr_index;			// color index
		void (GfxDebug::* cbFunction)();	// button callback
	};
	std::vector<BUTTON_NODE> vButton = {	// A=GREEN, B=TEAL, C=RED, D=MAGENTA, E=YELLOW
		{" Clr Breaks",		SDL_SCANCODE_C, 51, 62, 31, 0xC, &GfxDebug::cbClearBreaks},
		{"Reset",			SDL_SCANCODE_R,	43, 49, 31, 0xB, &GfxDebug::cbReset },
		{"NMI",				SDL_SCANCODE_N,	37, 41, 31, 0xD, &GfxDebug::cbNMI },
		{"IRQ",				SDL_SCANCODE_I,	31, 35, 31, 0xD, &GfxDebug::cbIRQ },
		{" FIRQ",			SDL_SCANCODE_F,	24, 29, 31, 0xD, &GfxDebug::cbFIRQ },
		{" RUN",			SDL_SCANCODE_D,	18, 22, 31, 0xA, &GfxDebug::cbRunStop },
	};

private:
	Bus* bus = nullptr;
	C6809* cpu = nullptr;
	GFX* gfx = nullptr;

	SDL_Texture* debug_texture = nullptr;	// debug screen texture
	std::vector<SDL_Texture*> glyph_textures;
	std::vector <Word> mem_bank = { 0x0000, 0x0400, 0x1800 };
	std::vector <Word> sDisplayedAsm;
	std::map<Word, bool> mapBreakpoints;
	std::list<Word> asmHistory;		// track last several asm addresses

	int csr_x = 0;
	int csr_y = 0;
	int csr_at = CSR_AT::CSR_AT_NONE;
	char mouse_wheel = 0;
	bool bSingleStep = DEBUG_SINGLE_STEP;	// false;
	bool bIsStepPaused = true;
	bool bIsCursorVisible = false;

	Word mousewheel_offset = 0;
	bool bMouseWheelActive = false;

	
};


#endif //__GFXDEBUG_H__


/*****

Notes:

	DEBUG_FLAGS: (Byte)	Debug Specific Hardware Flags

		bit 7)	Debug Enable
		bit 6)  Advance Single Step (on low to high edge)
		bit 5)	Single Step Enable
		bit 4)  Toggle Breakpoint at DEBUG_BRK_ADDRESS

		bit 3)  FIRQ  (on low to high edge)
		bit 2)  IRQ   (on low to high edge)
		bit 1)  NMI   (on low to high edge)
		bit 0)  RESET (on low to high edge)

	DEBUG_BRK_ADDR: (Word) Address of current breakpoint


***/
// * GfxMouse.cpp ***************************************
// *
// *  The Debugger
// ************************************

#include <map>
#include <list>
#include "types.h"
#include "Bus.h"
#include "C6809.h"
#include "GfxMode.h"
#include "GfxDebug.h"

#include "font8x8_system.h"

Byte GfxDebug::OnCallback(GfxMode* mode, Word ofs, Byte data, bool bWasRead)
{
	Bus* bus = Bus::getInstance();

	static Word _breakpoint = 0;
	
	if (bWasRead)
	{ // READ
		// printf("GfxDebug::OnCallback() =-->  READ\n");

		if (ofs == DBG_BRK_ADDR)	
		{
			data = (_breakpoint >> 8) & 0x00FF;
			bus->debug_write(ofs, data);
		}
		if (ofs == DBG_BRK_ADDR+1)	
		{
			data = _breakpoint & 0x00FF;
			bus->debug_write(ofs, data);
		}

		if (ofs == DBG_FLAGS)
		{
			data = 0;
			//      bit 7: Debug Visible
			if (gfx->DebugEnabled())	data |= 0x80;

			//      bit 6: Single Step Enable
			if (bSingleStep)			data |= 0x40;

			//		bit 5: clear all breakpoints (Write only)

			//		bit 4: Toggle Breakpoint at DEBUG_BRK_ADDRESS
			(mapBreakpoints[_breakpoint]) ? data |= 0x10 : data &= ~0x10;

			bus->debug_write(ofs, data);
		}

	}
	else
	{ // WRITE
		//printf("GfxDebug::OnCallback() =-->  WRITE\n");
		
		if (ofs == DBG_BRK_ADDR)
		{
			_breakpoint = (_breakpoint & 0x00ff) | (data << 8);
			bus->debug_write(ofs, data);
		}
		if (ofs == DBG_BRK_ADDR+1)
		{
			_breakpoint = (_breakpoint & 0xffff) | data;
			bus->debug_write(ofs, data);
		}
		if (ofs == DBG_FLAGS)
		{
			gfx->DebugEnabled( ((data & 0x80) == 0x80) );
			(data & 0x40) ? bSingleStep = true : bSingleStep = false;
			if (data & 0x20)	cbClearBreaks();
			if (data & 0x10)	mapBreakpoints[_breakpoint] = !mapBreakpoints[_breakpoint];
			if (data & 0x08)	this->cbFIRQ();
			if (data & 0x04)	this->cbIRQ();	
			if (data & 0x02)	this->cbNMI();	
			if (data & 0x01)	this->cbReset();	
		}
	}
	return data;
}

Word GfxDebug::MapDevice(MemoryMap* memmap, Word offset)
{
	std::string reg_name = "Debug System";
	DWord st_offset = offset;

	// map fundamental Debugger hardware registers:
	memmap->push({ offset, "", "" }); offset += 0;
	memmap->push({ offset, "", "Debugger Hardware Registers:" }); offset += 0;
	memmap->push({ offset, "DBG_BEGIN", "Start of Debugger Hardware Registers" }); offset += 0;

	memmap->push({ offset, "DBG_BRK_ADDR", "(Word) Address of current breakpoint" }); offset += 2;
	memmap->push({ offset, "DBG_FLAGS", "(Byte) Debug Specific Hardware Flags" }); offset += 1;
	memmap->push({ offset, "", ">    bit 7: Debug Enable" }); offset += 0;
	memmap->push({ offset, "", ">    bit 6: Single Step Enable" }); offset += 0;
	memmap->push({ offset, "", ">    bit 5: clear all breakpoints " }); offset += 0;
	memmap->push({ offset, "", ">    bit 4: Toggle Breakpoint at DEBUG_BRK_ADDRESS" }); offset += 0;
	memmap->push({ offset, "", ">    bit 3: FIRQ  (on low to high edge)" }); offset += 0;
	memmap->push({ offset, "", ">    bit 2: IRQ   (on low to high edge)" }); offset += 0;
	memmap->push({ offset, "", ">    bit 1: NMI   (on low to high edge)" }); offset += 0;
	memmap->push({ offset, "", ">    bit 0: RESET (on low to high edge)" }); offset += 0;

	memmap->push({ --offset, "DBG_END", "End of the Debugger Hardware Registers" }); offset += 1;
	memmap->push({ offset, "", "" });

	return offset - st_offset;
}


GfxDebug::GfxDebug()
{
	//printf("GfxDebug::GfxDebug()\n");

	bus = Bus::getInstance();
	gfx = bus->m_gfx;
	cpu = bus->m_cpu;
	cpu->debug = this;
	bus->gfxdebug = this;
}

GfxDebug::~GfxDebug()
{
	//printf("GfxDebug::~GfxDebug()\n");
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
	if (evnt->type == SDL_KEYDOWN) 
	{
		if (evnt->key.keysym.sym == SDLK_ESCAPE) 
		{
			bIsCursorVisible = false;
		}
		if (bIsCursorVisible)
		{
			if (evnt->key.keysym.sym == SDLK_LEFT || evnt->key.keysym.sym == SDLK_BACKSPACE)
				if (csr_x > 1)
					while (!CoordIsValid(--csr_x, csr_y));
			if (evnt->key.keysym.sym == SDLK_RIGHT)
				if (csr_x < 28)
					while (!CoordIsValid(++csr_x, csr_y));
			if (evnt->key.keysym.sym == SDLK_UP)
			{
				if (csr_y == 1)			mem_bank[0] -= 8;
				else if (csr_y == 11)	mem_bank[1] -= 8;
				else if (csr_y == 21)	mem_bank[2] -= 8;
				else if (csr_y > 1)		while (!CoordIsValid(csr_x, --csr_y));
			}
			if (evnt->key.keysym.sym == SDLK_DOWN)
			{
				if (csr_y == 9)			mem_bank[0] += 8;
				else if (csr_y == 19)	mem_bank[1] += 8;
				else if (csr_y == 29)	mem_bank[2] += 8;
				else if (csr_y < 30)	while (!CoordIsValid(csr_x, ++csr_y));
			}
			if (evnt->key.keysym.sym == SDLK_RETURN)
				bIsCursorVisible = false;
		}

		// SPACE advances single step
		if (evnt->key.keysym.sym == SDLK_SPACE)
		{			
			bSingleStep = true;
			bIsStepPaused = false;
			nRegisterBeingEdited.reg = GfxDebug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
		}

	}
	if (evnt->type == SDL_MOUSEWHEEL)
		mouse_wheel = evnt->wheel.y;

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
	if (!gfx->DebugEnabled())	return;


	const float delay = 1.0f / 30.0f;
	static float delayAcc = fElapsedTime;
	delayAcc += fElapsedTime;
	if (delayAcc >= delay)
	{
		delayAcc -= delay;
		SDL_SetRenderTarget(gfx->Renderer(), debug_texture);
		SDL_SetRenderDrawColor(gfx->Renderer(), 0, 0, 0, DEBUG_BACKGROUND_ALPHA);
		SDL_RenderClear(gfx->Renderer());

		MouseStuff();
		KeyboardStuff();

		DumpMemory(1, 1, mem_bank[0]);
		DumpMemory(1, 11, mem_bank[1]);
		DumpMemory(1, 21, mem_bank[2]);

		DrawCpu(39, 1);
		DrawCode(39, 6);

		DrawButtons();
		HandleButtons();

		if (!EditRegister(fElapsedTime))
			DrawCursor(fElapsedTime);

		SDL_SetRenderTarget(gfx->Renderer(), NULL);
	}

	//printf("CSR_FLAGS: $%02X\n", bus->read(CSR_FLAGS));
}

void GfxDebug::OnRender() 
{ 
	//printf("GfxDebug::OnRender()\n"); 
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
		SDL_RenderCopy(gfx->Renderer(), debug_texture, NULL, &dest);
	}
	else
		SDL_RenderCopy(gfx->Renderer(), debug_texture, NULL, NULL);
}

void GfxDebug::OutGlyph(int row, int col, Byte glyph, Byte red, Byte grn, Byte blu, bool bDropShadow)
{
	SDL_Rect dst = { row * 8, col * 8, 8, 8 };
	SDL_SetRenderTarget(gfx->Renderer(), debug_texture);
	if (bDropShadow)
	{
		SDL_Rect drop = { dst.x + 1, dst.y + 1, dst.w, dst.h };
		SDL_SetTextureColorMod(glyph_textures[glyph], 0, 0, 0);
		SDL_RenderCopy(gfx->Renderer(), glyph_textures[glyph], NULL, &drop);

	}
	SDL_SetTextureColorMod(glyph_textures[glyph], red, grn, blu);
	SDL_RenderCopy(gfx->Renderer(), glyph_textures[glyph], NULL, &dst);
}

int GfxDebug::OutText(int x, int y, std::string text, Byte red, Byte grn, Byte blu, bool bDropshadow)
{
	int pos = 0;
	for (auto& a : text)
		OutGlyph(x++, y, text[pos++], red, grn, blu, bDropshadow);
	return pos;
}

std::string GfxDebug::hex(Uint32 n, Uint8 d)
{
	std::string s(d, '0');
	for (int i = d - 1; i >= 0; i--, n >>= 4)
		s[i] = "0123456789ABCDEF"[n & 0xF];
	return s;
}


void GfxDebug::DumpMemory(int col, int row, Word addr)
{
	const bool use_debug_read = false;
	int line = 0;
	for (int ofs = addr; ofs < addr + 0x48; ofs += 8)
	{
		int c = col;
		std::string out = hex(ofs, 4) + " ";
		if (use_debug_read)
			for (int b = 0; b < 8; b++)
				out += hex(bus->debug_read(ofs + b), 2) + " ";
		else
			for (int b = 0; b < 8; b++)
				out += hex(bus->read(ofs + b), 2) + " ";

		c += OutText(col, row + line, out.c_str(), 224, 224, 255);

		bool characters = true;
		if (characters)
		{
			for (int b = 0; b < 8; b++)
			{
				Byte data;
				if (use_debug_read)
					data = bus->debug_read(ofs + b);
				else
					data = bus->read(ofs + b);
				OutGlyph(c++, row + line, data, 160, 160, 255);
			}
		}
		line++;
	}
}

void GfxDebug::DrawCpu(int x, int y)
{
	int RamX = x, RamY = y;
	// Condition Codes
	RamX += OutText(RamX, RamY, "CC($", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getCC(), 2).c_str(), 255, 255, 64);
	RamX += OutText(RamX, RamY, "): ", 64, 255, 64);
	if (cpu->getCC_E())		RamX += OutText(RamX, RamY, "E", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "e", 64, 255, 64);
	if (cpu->getCC_F())		RamX += OutText(RamX, RamY, "F", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "f", 64, 255, 64);
	if (cpu->getCC_H())		RamX += OutText(RamX, RamY, "H", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "h", 64, 255, 64);
	if (cpu->getCC_I())		RamX += OutText(RamX, RamY, "I", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "i", 64, 255, 64);
	if (cpu->getCC_N())		RamX += OutText(RamX, RamY, "N", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "n", 64, 255, 64);
	if (cpu->getCC_Z())		RamX += OutText(RamX, RamY, "Z", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "z", 64, 255, 64);
	if (cpu->getCC_V())		RamX += OutText(RamX, RamY, "V", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "v", 64, 255, 64);
	if (cpu->getCC_C())		RamX += OutText(RamX, RamY, "C", 255, 255, 64);
	else RamX += OutText(RamX, RamY, "c", 64, 255, 64);
	RamX = x; RamY++;	// carraige return(ish)

	// D = (A<<8) | B & 0x00FF
	RamX += OutText(RamX, RamY, "D:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getD(), 4), 255, 255, 64);
	RamX += OutText(RamX, RamY, " (A:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getA(), 2), 255, 255, 64);
	RamX += OutText(RamX, RamY, " B:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getB(), 2), 255, 255, 64);
	RamX += OutText(RamX, RamY, ")", 64, 255, 64);
	RamX = x; RamY++;	// carraige return(ish)

	// X
	RamX += OutText(RamX, RamY, " X:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getX(), 4), 255, 255, 64);
	// Y
	RamX += OutText(RamX, RamY, " Y:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getY(), 4), 255, 255, 64);
	// U
	RamX += OutText(RamX, RamY, " U:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getU(), 4), 255, 255, 64);
	RamX = x; RamY++;	// carraige return(ish)
	// PC
	RamX += OutText(RamX, RamY, "PC:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getPC(), 4), 255, 255, 64);
	// S
	RamX += OutText(RamX, RamY, " S:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getS(), 4), 255, 255, 64);
	// DP
	RamX += OutText(RamX, RamY, " DP:$", 64, 255, 64);
	RamX += OutText(RamX, RamY, hex(cpu->getDP(), 2), 255, 255, 64);
	RamX = x; RamY++;	// carraige return(ish)
}

void GfxDebug::DrawCode(int col, int row)
{
	int line = 0;
	Word top = cpu->getPC() + topOffset;
	std::string code = "";
	sDisplayedAsm.clear();
	while (row + line < 30)
	{
		bool atBreak = false;
		if (top < cpu->getPC())
		{
			if (mapBreakpoints[top])	atBreak = true;
			sDisplayedAsm.push_back(top);
			code = cpu->disasm(top, top);
			if (atBreak)
				OutText(col, row + line++, code, 160, 32, 32);
			else
				OutText(col, row + line++, code, 160, 160, 128);
		}
		else if (top == cpu->getPC())
		{
			if (mapBreakpoints[top])	atBreak = true;
			sDisplayedAsm.push_back(top);
			code = cpu->disasm(top, top);
			if (atBreak)
				OutText(col, row + line++, code, 255, 96, 96);
			else
				OutText(col, row + line++, code, 255, 255, 255);
		}
		else
		{
			if (mapBreakpoints[top])	atBreak = true;
			sDisplayedAsm.push_back(top);
			code = cpu->disasm(top, top);
			if (atBreak)
				OutText(col, row + line++, code, 192, 64, 64);
			else
				OutText(col, row + line++, code, 192, 192, 128);
		}
	}
}

bool GfxDebug::EditRegister(float fElapsedTime) 
{
	if (nRegisterBeingEdited.reg == EDIT_REGISTER::EDIT_NONE)
		return false;

	static float delay = 0.0625f;
	static float delayAcc = fElapsedTime;
	static int ci = 9;
	delayAcc += fElapsedTime;
	std::string ch = " ";

	if (delayAcc > delay + fElapsedTime)
	{
		delayAcc -= delay;
		ci++;
		if (ci > 15)	ci = 9;
	}
	OutGlyph(csr_x, csr_y, 0x8f, rand() % 256, rand() % 256, rand() % 256);

	Word data = 0;
	switch (nRegisterBeingEdited.reg) {
	case EDIT_CC:	data = (Word)cpu->getCC() << 8; break;
	case EDIT_D:	data = cpu->getD(); break;
	case EDIT_A:	data = (Word)cpu->getA() << 8; break;
	case EDIT_B:	data = (Word)cpu->getB() << 8; break;
	case EDIT_X:	data = cpu->getX(); break;
	case EDIT_Y:	data = cpu->getY(); break;
	case EDIT_U:	data = cpu->getU(); break;
	case EDIT_PC:	data = cpu->getPC(); bSingleStep = true;  topOffset = -20;  break;
	case EDIT_S:	data = cpu->getS(); break;
	case EDIT_DP:	data = (Word)cpu->getDP() << 8; break;
		//case EDIT_BREAK: data = breakpoint; break;
	}
	Byte digit = csr_x - nRegisterBeingEdited.x_min;
	Byte num = 0;
	if (digit == 0)	num = ((data & 0xf000) >> 12);
	if (digit == 1)	num = ((data & 0x0f00) >> 8);
	if (digit == 2)	num = ((data & 0x00f0) >> 4);
	if (digit == 3)	num = ((data & 0x000f) >> 0);
	ch = hex(num, 1);
	OutGlyph(csr_x, csr_y, ch[0], 0, 0, 0, false);

	// respond to numeric keys
	SDL_Keycode hx[] = { SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
						 SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
						 SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
						 SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F };
	char k[] = { '0', '1', '2', '3', '4', '5', '6', '7',
				 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char d[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	static bool state[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	const Uint8* keybfr = SDL_GetKeyboardState(NULL);
	Word n = 0;
	bool bKeyPressed = false;
	for (int t = 0; t < 16; t++)
	{
		if (state[t] == 0 && keybfr[hx[t]])
		{
			n = d[t];
			bKeyPressed = true;
			state[t] = 1;
		}
		// reset the key
		if (state[t] == 1 && !keybfr[hx[t]])	state[t] = 0;
	}

	if (bKeyPressed)
	{
		//printf("pressed\n");
		if (digit == 0)		data = (data & 0x0fff) | (n << 12);
		if (digit == 1)		data = (data & 0xf0ff) | (n << 8);
		if (digit == 2)		data = (data & 0xff0f) | (n << 4);
		if (digit == 3)		data = (data & 0xfff0) | (n << 0);

		switch (nRegisterBeingEdited.reg) {
		case EDIT_CC:	cpu->setCC(data >> 8);  break;
		case EDIT_D:	cpu->setD(data);		break;
		case EDIT_A:	cpu->setA(data >> 8);	break;
		case EDIT_B:	cpu->setB(data >> 8);	break;
		case EDIT_X:	cpu->setX(data);		break;
		case EDIT_Y:	cpu->setY(data);		break;
		case EDIT_U:	cpu->setU(data);		break;
		case EDIT_PC:	cpu->setPC(data);		break;
		case EDIT_S:	cpu->setS(data);		break;
		case EDIT_DP:	cpu->setDP(data >> 8);  break;
			//case EDIT_BREAK:	breakpoint = data;  break;
		}
		if (csr_x < nRegisterBeingEdited.x_max)
			csr_x++;
	}
	SDL_Keycode ex[] = { SDL_SCANCODE_LEFT , SDL_SCANCODE_RIGHT , SDL_SCANCODE_RETURN , SDL_SCANCODE_ESCAPE };
	static bool bEx[] = { 0, 0, 0, 0 };
	for (int t = 0; t < 4; t++)
	{
		if (bEx[t] == 0 && keybfr[ex[t]])
		{
			// left
			if (ex[t] == SDL_SCANCODE_LEFT)
				if (csr_x > nRegisterBeingEdited.x_min)
					csr_x--;
			// right
			if (ex[t] == SDL_SCANCODE_RIGHT)
				if (csr_x < nRegisterBeingEdited.x_max)
					csr_x++;
			// enter updates and saves the register
			if (ex[t] == SDL_SCANCODE_RETURN || ex[t] == SDL_SCANCODE_ESCAPE)
				nRegisterBeingEdited.reg = EDIT_REGISTER::EDIT_NONE;
			bEx[t] = 1;
		}
		else if (bEx[t] == 1 && !keybfr[ex[t]])
			bEx[t] = 0;
	}
	return true;
}



void GfxDebug::DrawCursor(float fElapsedTime)
{
	if (!bIsCursorVisible)	return;

	std::string ch = " ";

	// output the cursor
	OutGlyph(csr_x, csr_y, 0x8f, rand() % 256, rand() % 256, rand() % 256, true);

	// which character is under the cursor?
	switch (csr_at)
	{
	case CSR_AT::CSR_AT_ADDRESS:
	{
		Word addr = 0;
		if (csr_y < 10) addr = mem_bank[0] + ((csr_y - 1) * 8);
		else if (csr_y < 20) addr = mem_bank[1] + ((csr_y - 11) * 8);
		else if (csr_y < 30) addr = mem_bank[2] + ((csr_y - 21) * 8);
		Byte digit = csr_x - 1;
		Byte num = 0;
		if (digit == 0)	num = ((addr & 0xf000) >> 12);
		if (digit == 1)	num = ((addr & 0x0f00) >> 8);
		if (digit == 2)	num = ((addr & 0x00f0) >> 4);
		if (digit == 3)	num = ((addr & 0x000f) >> 0);
		ch = hex(num, 1);
		break;
	}
	case CSR_AT::CSR_AT_DATA:
	{
		Byte ofs = ((csr_x - 5) / 3) + ((csr_y - 1) * 8);
		Byte digit = ((csr_x + 1) % 3) - 1;
		Byte num = 0;
		Word addr = mem_bank[0];
		if (csr_y > 10 && csr_y < 20) { ofs -= 80; addr = mem_bank[1]; }

		if (csr_y > 20) { ofs -= 160; addr = mem_bank[2]; }


		Byte data = bus->debug_read(addr + ofs);
		if (digit == 0) num = (data & 0xf0) >> 4;
		if (digit == 1) num = (data & 0x0f) >> 0;
		ch = hex(num, 1);
		break;
	}

	}
	// display the reversed character
	OutGlyph(csr_x, csr_y, ch[0], 0, 0, 0, false);
}

bool GfxDebug::SingleStep()
{
	// do nothing if singlestep is disabled
	if (!bSingleStep)
		return true;
	// wait for space
	if (bIsStepPaused)
	{
		// bIsStepPaused = bSingleStep;
		return false;
	}
	return true;
}

void GfxDebug::ContinueSingleStep() {
	// if breakpoint reached... enable singlestep
	if (mapBreakpoints[cpu->getPC()] == true)
	{
		gfx->DebugEnabled(true);
		bSingleStep = true;
	}
	// continue from paused state?
	bIsStepPaused = bSingleStep;
	topOffset = -25;
}


void GfxDebug::MouseStuff()
{
	int mx = bus->read_word(CSR_XPOS) / 8;
	int my = bus->read_word(CSR_YPOS) / 8;
	Uint32 btns = SDL_GetRelativeMouseState(NULL, NULL);
	// mouse wheel
	if (mouse_wheel)
	{
		// scroll memory banks
		if (mx > 0 && mx < 29)
		{
			if (my > 0 && my < 10)	mem_bank[0] -= mouse_wheel * 8;
			if (my > 10 && my < 20)	mem_bank[1] -= mouse_wheel * 8;
			if (my > 20 && my < 30)	mem_bank[2] -= mouse_wheel * 8;
			bIsCursorVisible = false;
		}
		// scroll the code
		if (mx > 38 && mx < 64 && my > 5 && my < 30)
		{
			bSingleStep = true;	// scrollwheel enters into single step mode
			nRegisterBeingEdited.reg = GfxDebug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
			topOffset -= mouse_wheel * 1;		// slow scroll
			if (SDL_GetModState() & KMOD_CTRL)	// is CTRL down?
				topOffset -= mouse_wheel * 3;	// faster scroll

			if (SDL_GetModState() & KMOD_ALT)	// is ALT down?
				bSingleStep = false;	// alt+scrollwheel turns off single step mode
		}
		// reset the wheel
		mouse_wheel = 0;
	}
	// left mouse button clicked?
	static bool last_LMB = false;
	if (btns & 1 && !last_LMB)
	{
		// click to select
		if (btns & 1)
		{
			if (CoordIsValid(mx, my))
			{
				csr_x = mx;
				csr_y = my;
				bIsCursorVisible = true;
			}
			else
				bIsCursorVisible = false;
			//printf("MX:%d  MY:%d\n", mx, my);
		}
		// condition code clicked?
		if (my == 1)
		{
			if (mx == 48) cpu->setCC_E(!cpu->getCC_E());
			if (mx == 49) cpu->setCC_F(!cpu->getCC_F());
			if (mx == 50) cpu->setCC_H(!cpu->getCC_H());
			if (mx == 51) cpu->setCC_I(!cpu->getCC_I());
			if (mx == 52) cpu->setCC_N(!cpu->getCC_N());
			if (mx == 53) cpu->setCC_Z(!cpu->getCC_Z());
			if (mx == 54) cpu->setCC_V(!cpu->getCC_V());
			if (mx == 55) cpu->setCC_C(!cpu->getCC_C());
		}
		// Register Clicked?
		bool bFound = false;
		for (auto& a : register_info)
		{
			if (a.y_pos == my && mx >= a.x_min && mx <= a.x_max)
			{
				// begin editing a register
				nRegisterBeingEdited.reg = a.reg;
				nRegisterBeingEdited.value = a.value;
				nRegisterBeingEdited.y_pos = a.y_pos;
				nRegisterBeingEdited.x_min = a.x_min;
				nRegisterBeingEdited.x_max = a.x_max;
				csr_x = mx;
				csr_y = my;
				bFound = true;
			}
		}
		if (!bFound)
			nRegisterBeingEdited.reg = EDIT_NONE;
		// left-click on code line toggles breakpoint
		if (mx > 38 && mx < 64 && my > 5 && my < 30 && bSingleStep)
		{
			Word offset = sDisplayedAsm[my - 6];
			(mapBreakpoints[offset]) ?
				mapBreakpoints[offset] = false :
				mapBreakpoints[offset] = true;
		}
	}
	last_LMB = (btns & 1);
	// right mouse button clicked
	static bool last_RMB = false;
	if (btns & 4 && !last_RMB)
	{
		// on PC register
		if (my == 4 && mx > 42 && mx < 47)
		{
			bSingleStep = !bSingleStep;
			if (!bSingleStep)
				nRegisterBeingEdited.reg = GfxDebug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
		}
		// right-click on code line toggles breakpoint and resumes execution
		if (mx > 38 && mx < 64 && my > 5 && my < 30 && bSingleStep)
		{
			Word offset = sDisplayedAsm[my - 6];
			(mapBreakpoints[offset]) ?
				mapBreakpoints[offset] = false :
				mapBreakpoints[offset] = true;
			if (mapBreakpoints[offset] == true)
				bSingleStep = false;
		}
	}
	last_RMB = (btns & 4);
}

bool GfxDebug::CoordIsValid(int x, int y)
{
	if (y > 0 && y < 30 && y != 10 && y != 20)
	{
		// at an address
		if (x > 0 && x < 5)
		{
			csr_at = CSR_AT::CSR_AT_ADDRESS;
			return true;
		}
		// at a data entry
		if (x > 5 && x < 29 && (x + 1) % 3)
		{
			csr_at = CSR_AT::CSR_AT_DATA;
			return true;
		}
	}
	return false;
}

void GfxDebug::KeyboardStuff()
{
	if (!bIsCursorVisible)	return;

	SDL_Keycode hx[] = { SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
						 SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
						 SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
						 SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F };
	char k[] = { '0', '1', '2', '3', '4', '5', '6', '7',
				 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char d[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	static bool state[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	const Uint8* keybfr = SDL_GetKeyboardState(NULL);

	// check for valid key presses
	bool bKeyPressed = false;
	Byte ch = 0;
	for (int t = 0; t < 16; t++)
	{
		if (state[t] == 0 && keybfr[hx[t]])
		{
			state[t] = 1;
			bKeyPressed = true;
			ch = d[t];
			// printf("%c\n", k[t]);
		}
		// reset the key
		if (state[t] == 1 && !keybfr[hx[t]])	state[t] = 0;
	}

	// respond to [DEL]
	// ...

	if (!bKeyPressed)	return;

	//printf("$%1x\n", ch);
	switch (csr_at)
	{
		case CSR_AT::CSR_AT_ADDRESS:
		{
			Word addr = 0;
			if (csr_y < 10) addr = mem_bank[0] + ((csr_y - 1) * 8);
			else if (csr_y < 20) addr = mem_bank[1] + ((csr_y - 11) * 8);
			else if (csr_y < 30) addr = mem_bank[2] + ((csr_y - 21) * 8);
			Byte digit = csr_x - 1;
			if (digit == 0)	addr = (addr & 0x0fff) | (ch << 12);
			if (digit == 1)	addr = (addr & 0xf0ff) | (ch << 8);
			if (digit == 2)	addr = (addr & 0xff0f) | (ch << 4);
			if (digit == 3)	addr = (addr & 0xfff0) | (ch << 0);
			if (csr_y < 10)			mem_bank[0] = addr - ((csr_y - 1) * 8);
			else if (csr_y < 20)	mem_bank[1] = addr - ((csr_y - 11) * 8);
			else if (csr_y < 30)	mem_bank[2] = addr - ((csr_y - 21) * 8);
			if (csr_x < 5)	while (!CoordIsValid(++csr_x, csr_y));
			break;
		}
		case CSR_AT::CSR_AT_DATA:
		{
			Byte ofs = ((csr_x - 5) / 3) + ((csr_y - 1) * 8);
			Byte digit = ((csr_x + 1) % 3) - 1;
			Word addr = mem_bank[0];
			if (csr_y > 10 && csr_y < 20) { ofs -= 80; addr = mem_bank[1]; }
			if (csr_y > 20) { ofs -= 160; addr = mem_bank[2]; }
			Byte data = bus->debug_read(addr + ofs);
			if (digit == 0)		data = (data & 0x0f) | (ch << 4);
			if (digit == 1)		data = (data & 0xf0) | (ch << 0);
			bus->write(addr + ofs, data);
			if (csr_x < 28)		while (!CoordIsValid(++csr_x, csr_y));
			break;
		}
	}
}


void GfxDebug::DrawButtons()
{
	// change the run/stop according to the single step state
	if (bSingleStep)
	{
		vButton[5].text = " RUN";
		vButton[5].x_min = 18;
		vButton[5].clr_index = 0xA;
	}
	else
	{
		vButton[5].text = " STOP";
		vButton[5].x_min = 17;
		vButton[5].clr_index = 0xE;
	}
	// draw the buttons
	for (auto& a : vButton)
	{
		int x1 = a.x_min;
		int x2 = a.x_max;
		int y = a.y_pos;
		// background
		int p = a.clr_index % 16;
		for (int x = x1; x <= x2; x++)
			OutGlyph(x, y, 0x8f, gfx->red(p), gfx->grn(p), gfx->blu(p), true);
		// foreground
		int sX = x1 + ((x2 - x1) / 2) - (int)a.text.size() / 2;
		OutText(sX, y, a.text, 0, 0, 0, false);
	}
}

void GfxDebug::HandleButtons()
{
	// bus->write(GFX_MODE_INDEX, 7);	// Sete the GfxMode Index to GfxSystem PAGE
	int mx = bus->read_word(CSR_XPOS) / 8;
	int my = bus->read_word(CSR_YPOS) / 8;
	Uint32 btns = SDL_GetRelativeMouseState(NULL, NULL);

	static bool last_LMB = false;
	if (btns & 1 && !last_LMB)
	{
		for (auto& a : vButton)
		{
			if (my == a.y_pos && mx >= a.x_min && mx <= a.x_max)
			{
				//printf("Click %s\n", a.text.c_str());
				if (a.cbFunction)	(this->*a.cbFunction)();
			}
		}
	}
	last_LMB = (btns & 1);
}

void GfxDebug::cbClearBreaks()
{
	if (mapBreakpoints.size() > 0)
		mapBreakpoints.clear();
}
void GfxDebug::cbReset()
{
	cpu->reset();
	topOffset = 0;
}
void GfxDebug::cbNMI()
{
	cpu->nmi();
	bIsStepPaused = false;
}
void GfxDebug::cbIRQ()
{
	cpu->irq();
	bIsStepPaused = false;
}
void GfxDebug::cbFIRQ()
{
	cpu->firq();
	bIsStepPaused = false;
}

void GfxDebug::cbRunStop()
{
	//printf("RunStop: %d\n", bSingleStep);

	(bSingleStep) ? bSingleStep = false : bSingleStep = true;
	bIsStepPaused = bSingleStep;
	nRegisterBeingEdited.reg = GfxDebug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
}

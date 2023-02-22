// main.cpp
// windows build: use Visual C++
//
// linux build:   g++ -Werror -pthread -std=c++17 *.cpp -o main -lSDL2 -DLINUX
// linux run:     ./main
//
// VSCode:  Git Extension Added
// 
// git push:
//		git add .
//		git commit -m "COMMIT_DESCRIPTION"
//		git push
//
// git pull:
//		git pull 
//
// Copyright (C) 2023 by Jay Faries


#include "types.h"
#include "Bus.h"
//#include "GFX.h"
//#include "GfxMode.h"
//#include "GfxGlyph64.h"

int main(int argc, char* argv[])
{
	//printf("Starting the TPR_6809 Application...\n");	
	//printf("\n");
	//printf("Compiled: %s\n", __TIMESTAMP__);		// __TIMESTAMP__  __DATE__


	Bus* bus = Bus::getInstance();
	bus->run();


    return 0;
}




/*

TO-DO:
	- GfxSprite: Framework is present, but still needs to be completely fleshed out.
	- Math: Framework is present, but still has little to no function. Needs a lot
		of work. The "Half" precision library does work, but still needs to 
		support "full" (or Single) precision. An implementation the shares registers
		between half and full precision seems very sloppy. Reconsider dedicating
		3x (16-bit) registers for "Half" and 3x (32-bit) registers for "Full".

	- FileIO: Framework is present, has some functionality, but still needs work.

	- Audio/Sound (WAV/FM): Not even started, needs implementation. Should run within 
		its own task, possibly just a function of clock timing. Similar to how the
		CPU is currently being clocked, the audio should have a base clock. In other
		words, decide whether to use a modern audio chip as hardware or to emulate
		old hardware in software, possibly using a third raspberry PI PICO.
		AY-3-8910 (no longer made): https://f.rdw.se/AY-3-8910-datasheet.pdf
		
		A simple two-channel DAC with a dedicated audio task may be only maintainable
		option. Sourcing Modern SSG (Software-controlled Sound Generation) chips is
		proving to be somewhat of a challenge.
			Waveforms:
				- Sine
				- Square
				- Sawtooth
				- Triangle
				- Noise
			Online waveform examples: https://www.rapidtables.com/tools/tone-generator.html?f=400

	- MemoryBank Paging: Still needs to be fully implemented. Consider, using
		external 2MB QSPI ISSI 16Mbit SerialRAM (see notes below) and/or some of
		either/both PICO's onboard flash RAM as a kind of "persistent" memory.
		https://www.mouser.com/ProductDetail/ISSI/IS66WVS2M8BLL-104NLI?qs=doiCPypUmgFx786bHGqGiQ%3D%3D

	- GPIO/Parallel I/O: Still needs to be fully implemented. At least 16-bits of
		input/output pins should be available to the user on the final hardware. The
		method is still undecided, possibly use the serial to parallel chip that is
		used in one of the Landboards PicoMite VGA versions. The MCP23017/MCP23S17 or
		MCP23017-E/SP
		see: 
		https://www.digikey.com/en/products/detail/microchip-technology/MCP23017-E-SP/894272
		https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf
		http://land-boards.com/blwiki/index.php?title=PiPicoMite02#SPI_port_expanders



Raspberry PI Pico Retro 6809 Thing Notes:

Using two PI PICO's:
	"TONI" PICO #1 -- CORE ONE:	(6809 CPU Emulation / Debugger)
		- 6809 CPU emulation
			- Integrated Debugger
		- Main 64KB Memory Map
		
			- Receives Memory Event Dispatches from the other PICO as paged memory banks
			- Updates Hardware Registers via UART, SPI, or I2C @ 115200 baud 
				as masater with PICO #2 acting as slave.
	PICO #1 -- CORE TWO:	(Graphics Display)
		- Main Video Display
			- Aspect Ratio 16/10 or 1.6f with main video timing set to 
				512x320 (emulation runs at 1536x960, 1024x640, or 512x320)
		- Main Video Display Buffer (5k total)
			- GLYPH MODE
				- pixel resolution = 512x320
				- Font Size = 8x8
				- data buffer size = 5 kilobytes total
					- Glyph Data Buffer size = 2.5kb
					- Color Attribute Data Buffer size = 2.5kb
			- TILE16x16 MODE
				- pixel resolution = 512x320
				- 32x20 on screen ties
					- 640 bytes visible screen data but can be offset to 
						anywhere within the rest of the video display.
					- can use an x/y pixel offset for smooth scrolling
			- GRAPHICS MODE (5k buffer)
				- pixel resolutions
					- 256x160 x 2-Color
					- 256x80 x 4-Color
					- 128x160 x 4-Color
					- 128x80 x 16-Color

	"KIMI" PICO #2 -- CORE ONE: 	(Device Events)
		- Handles / Emulates all devices attached via the "Bus" to PICO 1-CORE ONE
			- Primary Event Dispatcher
				- PS/2 Keyboard
				- PS/2 Mouse
				- Two GameControllers/Joysticks
				- UART/SPI/QSPI/I2C Serial Communications with the other PICO
				- USB Serial Communications 
				- Extended Memory Paged Memory (using available remaining memory from this PICO)

	PICO #2 -- CORE TWO: 	(Audio Events)
		- Handles Audio Events
			- WAV audio buffers
			- Parallel WAV output for simultaneous playing of WAVs and FM voices
		- 5 FM Synthesizer voices
			- Sine, Square, Triangle, and Noise waveforms
				- Attack, Sustain, Decay
				- Volume
				- Etc


	Memory Bank Selects (external 2MB QSPI ISSI 16Mbit SerialRAM):
	https://www.mouser.com/ProductDetail/ISSI/IS66WVS2M8BLL-104NLI?qs=doiCPypUmgFx786bHGqGiQ%3D%3D

	Posibly include a AY-3-8910 Programmable Sound Generator as was used in the 
	TRS-80 Color Computer sound generator cartridge. 
	https://f.rdw.se/AY-3-8910-datasheet.pdf





		
*/

// main.cpp
// windows build: use Visual C++
//
// linux build:   g++ -Werror -std=c++17 *.cpp -o main -lSDL2 -DLINUX
// linux run:     ./main
//

#include "dep_SDL.h"
#include "Bus.h"

int main(int argc, char* argv[])
{
    printf("Starting the TPR_6809 Application...\n");	
	printf("Chunky Tom doesn't quit fit in the door!.\n"); 
	printf("\n");
	printf("********************\n\n");
	printf("Changed by PI400\n");
	printf("********************\n\n");

	Bus* bus = Bus::getInstance();
	bus->run();

    return 0;
}




/*

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
				512x320 (emulation runs at 1536x960 or 1280x800)
		- Main Video Display Buffer
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


		
		
*/
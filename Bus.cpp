// Bus.cpp
//
#include "types.h"
#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include "Device.h"
#include "GFX.h"
#include "MemoryMap.h"      // move this to the Memory.h when available
#include "Bus.h"

// initialize staatics

Bus* Bus::s_instance = nullptr;
bool Bus::s_bIsRunning = false;
//GFX* Bus::s_gfx = nullptr;

//// private /////////////////////////

Bus::Bus()
{
    //printf("Bus::Bus()\n");
    // Initialize SDL, TTF, IMAGE, MIXER, etc...
    SDL_Init(SDL_INIT_EVERYTHING);

    // Initialize the Bus static object    
    s_bIsRunning = true;
    std::atexit(_OnQuit);

    m_fps = 0;

    // open memory mapping
    MemoryMap* memmap = new MemoryMap();
    Word mem_offset = memmap->start();

    // Create all of the attached devices:
    {
        // create the graphics device:
        m_gfx = new GFX();
        if (m_gfx)  _devices.push_back(m_gfx);

        // add more devices here:
        // ...
    }

    // map the memory for all attached devices
    for (auto& a : _devices)
        mem_offset = a->MapDevice(memmap, mem_offset);

    // close memory mapping
    mem_offset = memmap->end(mem_offset);
    delete memmap;
}
Bus::~Bus() 
{
    //printf("Bus::~Bus()\n");

    // finally, destroy all of the attached devices
	if (_devices.size()>0)
		_devices.clear();

	// final shutdown of SDL, TTF, IMAGE, MIXER, etc...
	SDL_Quit();
}

std::string Bus::hex(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
};

bool Bus::Err(const char* msg)
{
    printf("\n\nERROR: %s\n\n", msg);
    if (s_instance != nullptr)
    {
        GFX* gfx = s_instance->m_gfx;
        if (gfx == nullptr)
        {
            printf("%s\n", msg);
            s_instance->s_bIsRunning = false;
            return false;
        }
        SDL_Window* ptrWindow = gfx->_window;
        if (ptrWindow == nullptr)
        {
            printf("%s\n", msg);
            s_instance->s_bIsRunning = false;
            return false;
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR!!", msg, ptrWindow);
        s_instance->s_bIsRunning = false;
    }
    // exit(999);
    s_instance->s_bIsRunning = false;
    return false;
}
void Bus::_OnInitialize() 
{
    //printf("Bus::_OnInitialize()\n");

	// call OnEvent for each device
	for (auto &a : s_instance->_devices)
		a->OnInitialize();
}
void Bus::_OnEvent(SDL_Event* evnt) 
{
    //printf("Bus::_OnEvent(%d)\n", evnt->type);

	// call OnEvent for each device
	for (auto &a : s_instance->_devices)
		a->OnEvent(evnt);
}
void Bus::_OnCreate() 
{
    //printf("Bus::_OnCreate()\n");

	// call OnCreate for each device
	for (auto &a : s_instance->_devices)
		a->OnCreate();
}
void Bus::_OnDestroy() 
{ 
    //printf("Bus::_OnDestroy()\n");

	// call OnDestroy for each device
	for (auto &a : s_instance->_devices)
		a->OnDestroy();
}
void Bus::_OnUpdate() 
{
    //printf("Bus::_OnUpdate()\n");

    // Handle Timing
    static std::chrono::time_point<std::chrono::system_clock> tp1 = std::chrono::system_clock::now();
    static std::chrono::time_point<std::chrono::system_clock> tp2 = std::chrono::system_clock::now();

    tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = tp2 - tp1;
    tp1 = tp2;
    // Our time per frame coefficient
    float fElapsedTime = elapsedTime.count();
    static float fLastElapsed = fElapsedTime;
    static float fFrameTimer = fElapsedTime;

    // count frames per second
    static int fps = 0;
    static int frame_count = 0;
    static float frame_acc = fElapsedTime;
    frame_count++;
    frame_acc += fElapsedTime;    
    if (frame_acc > 1.0f + fElapsedTime)
    {
        frame_acc -= 1.0f;
		s_instance->m_fps = frame_count;

		// std::string title = "FPS: " + std::to_string(frame_count);
		// SDL_SetWindowTitle(s_instance->window, title.c_str());
		frame_count = 0;
    }

	// update the devices
	for (auto &a : s_instance->_devices)
		a->OnUpdate(fElapsedTime);
}
void Bus::_OnRender() 
{
    //printf("Bus::_OnRender()\n");

	for (auto &a : s_instance->_devices)
		a->OnRender();
}
void Bus::_OnQuit()
{
    //printf("Bus::_OnQuit()\n");

	// call OnQuit for the decices
	for (auto &a : s_instance->_devices)
		a->OnQuit();
    s_instance->_devices.clear();
}



//// public /////////////////////////

Bus* Bus::getInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new Bus();
        return s_instance;
    }
    return s_instance;
}

void Bus::run()
{
    //printf("Bus::run()\n");

    // call OnInitialize() for all devices
    // (this is called after all device objects are created)
    _OnInitialize();

    // call OnCreate() for all devices
    // (this may be called in program. Such as gain/lost focus
    //  or graphics mode changes.)
    _OnCreate();

    while(s_bIsRunning)
    {
        SDL_Event evnt;
        while(SDL_PollEvent(&evnt))
        {
            switch(evnt.type)
            {
                case SDL_QUIT:
                {
                    s_bIsRunning = false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    if (evnt.key.keysym.sym == SDLK_ESCAPE)
                        s_bIsRunning = false;
                    break;
                }
            }
            // call OnEvent(SDL_Event& evnt) for all devices
            _OnEvent(&evnt);
        }

        // call OnUpdate(float fElapsedTim) for all devices
        _OnUpdate();

        // call OnRender() for all devices
        _OnRender();
    }

    // call OnDestroy() for all devices
    _OnDestroy();

    // call OnQuit() for all devices
    _OnQuit();

}

int Bus::getFPS() 
{    
	return s_instance->m_fps;
}

// Bus.cpp
//
#include "types.h"
#include <chrono>
#include <thread>
#include <cstdlib>
#include <stdio.h>
#include "Device.h"
#include "GFX.h"
#include "MemoryMap.h"      // move this to the Memory.h when available
#include "Memory.h"
#include "C6809.h"
#include "Bus.h"

// initialize staatics

Bus* Bus::s_instance = nullptr;
bool Bus::s_bIsRunning = false;

//// private /////////////////////////

Bus::Bus()
{
    s_instance = this; 

    //printf("Bus::Bus()\n");
    // Initialize SDL, TTF, IMAGE, MIXER, etc...
    SDL_Init(SDL_INIT_EVERYTHING);

    // Initialize the Bus static object    
    s_bIsRunning = true;
    std::atexit(_OnQuit);

    m_fps = 0;

    // open memory mapping
    MemoryMap* memmap = new MemoryMap();
    DWord mem_offset = memmap->start();

    // Create all of the attached devices:

    // create the memory map device:
    m_memory = new Memory();
    _devices.push_back(m_memory);

    // create the cpu (clocked via external thread)    
    bCpuEnabled = false;
    m_cpu = new C6809(this);    
    // CPU not attached to _devices vector

    //// Memory-Mapped Devices:
    // 
    // map low RAM $0000-$17ff (VIDEO_END)
    m_memory->AssignRAM("Low RAM", 0x1800);
    m_memory->bus = this;
    mem_offset = 0x1800;

    // create the graphics device:

    // PROBLEM: need m_gfx to be "defined before it's defined" for size ...
    m_gfx = new GFX();
    int size = m_gfx->MapDevice(memmap, mem_offset);
    m_gfx->Base(mem_offset);
    m_gfx->Size(size);
    mem_offset += size;
    m_memory->AssignREG("GFX_DEVICE", size, GFX::OnCallback);
    REG* reg = m_memory->FindRegByName("GFX_DEVICE");
    //m_gfx = new GFX(reg->Base(), reg->Size());
    _devices.push_back(m_gfx);
    m_memory->ReassignReg(reg->Base(), m_gfx, reg->Name(), reg->Size(), reg->callback);


    
    // add more devices here:
    // ...



    //// map memory for all attached devices
    //for (auto& a : _devices)
    //    mem_offset = a->MapDevice(memmap, mem_offset);




    // Reserve RAM to fill in vacancy where the hardware registers lack
    // this should be close to zero after all of the devices are mapped.
    // System RAM starts at $2000. This should reserve memory unused by
    // the hardware register devices. There should be around 2k memory
    // available to be mapped by register devices.
    int gfx_size = 0x2000 - mem_offset;
    mem_offset += m_memory->AssignRAM("HDW_RESERVE", gfx_size);

    // close memory mapping
    mem_offset += memmap->end(mem_offset);    
    delete memmap;

    // map temporary
    mem_offset += m_memory->AssignRAM("System RAM", 0x8000);
    mem_offset += m_memory->AssignRAM("RAM_BANK_1", 0x2000);
    mem_offset += m_memory->AssignRAM("RAM_BANK_2", 0x2000);

    std::string rom_path = ".\\asm\\rom_e000.hex";
    #ifdef LINUX 
        rom_path = "./asm/rom_e000.hex";
    #endif
    mem_offset += m_memory->AssignROM("BIOS_ROM", 0x2000, rom_path.c_str());


    // Memory Device Allocation ERROR???
    if (mem_offset != 0x10000) {
        printf("ERROR: \n  Memory::AssignMemory() failed to fill map to 0xFFFF!" \
            " $%04X bytes remain.\n", 0x10000 - mem_offset);
        std::string err = "$" + hex(0x10000 - mem_offset, 4) + " bytes remain.";

        Bus::Err("Memory::AssignMemory() failed to fill map to 0xFFFF!");        
        // Bus::getInstance()->IsRunning(false);
        for (auto& a : m_memory->m_memBlocks) {
            printf("[%s] \t$%04X-$%04X $%04X Bytes\n", a->Name(), a->Base(), (a->Base() + a->Size() - 1), a->Size());
        }
        return;
    }
    for (auto& a : m_memory->m_memBlocks) {
        printf("[%s] \t$%04X-$%04X $%04X Bytes\n", a->Name(), a->Base(), (a->Base() + a->Size() - 1), a->Size());
    }
    printf("\n");
    //printf("Final Memory Offset: $%08X\n\n", mem_offset);

    Word tw = this->read_word(SCR_WIDTH);
    Word th = this->read_word(SCR_HEIGHT);
    printf("window width: %d    window height: %d\n", tw, th);
    Word pw = this->read_word(PIX_WIDTH);
    Word ph = this->read_word(PIX_HEIGHT);
    printf("pixel width: %d    pixel height: %d\n", pw, ph);


    bCpuEnabled = true;
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
		frame_count = 0;
    }

	// update the devices
	for (auto &a : s_instance->_devices)
		a->OnUpdate(fElapsedTime);
}
// void Bus::_OnRender() 
// {
//     //printf("Bus::_OnRender()\n");

// 	for (auto &a : s_instance->_devices)
// 		a->OnRender();
// }
void Bus::_OnQuit()
{
    //printf("Bus::_OnQuit()\n");

	// call OnQuit for the decices
	for (auto &a : s_instance->_devices)
		a->OnQuit();
    s_instance->_devices.clear();
}

Byte Bus::read(Word offset) {
    if (m_memory)
        return m_memory->read(offset);
    return 0xcc;
}
void Bus::write(Word offset, Byte data) {
    if (m_memory)
        m_memory->write(offset, data);
}

Word Bus::read_word(Word offset) {
    Word data = (read(offset) << 8) & 0xFF00;
    data |= read(offset + 1);
    return data;
}

void Bus::write_word(Word offset, Word data) {
    write(offset, (data & 0xff00) >> 8);
    write(offset + 1, data & 0xff);
}

Byte Bus::debug_read(Word offset)
{
    return m_memory->debug_read(offset);
}

void Bus::debug_write(Word offset, Byte data)
{
    m_memory->debug_write(offset, data);
}

Word Bus::debug_read_word(Word offset)
{
    return m_memory->debug_read_word(offset);
}

void Bus::debug_write_word(Word offset, Word data)
{
    m_memory->debug_write_word(offset, data);
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


void Bus::CpuThread()
{
    Bus* bus = Bus::getInstance();
    while (bus->IsRunning())
    {
        using clock = std::chrono::system_clock;
        using sec = std::chrono::duration<double, std::nano>;
        static auto before_CPU = clock::now();
        const sec duration = clock::now() - before_CPU;
        if (duration.count() > 500.0f) {		// 1000.f = 1mhz, 500.0f = 2mhz
            before_CPU = clock::now();
            // dont send clocks while changing resolution modes
            //if (!bus->gfx_restarting)
            if (bus->bCpuEnabled)
                s_instance->m_cpu->clock();
        }
    }
}
void Bus::run()
{
    //printf("Bus::run()\n");

    // /start the CPU thread
    std::thread th = std::thread(&Bus::CpuThread);
    // SDL_CreateThread()

    // call OnInitialize() for all devices
    // (this is called after all device objects are created)
    _OnInitialize();

    while (s_bIsRunning)
    {
        // call OnCreate() for all devices
        // (this may be called in program. Such as gain/lost focus
        //  or graphics mode changes.)
        _OnCreate();

        while (!m_gfx->bIsDirty)
        {
            SDL_Event evnt;
            while (SDL_PollEvent(&evnt))
            {
                switch (evnt.type)
                {
                case SDL_QUIT:
                {
                    s_bIsRunning = false;
                    m_gfx->bIsDirty = true;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    if (evnt.key.keysym.sym == SDLK_ESCAPE)
                    {
                        s_bIsRunning = false;
                        m_gfx->bIsDirty = true;
                    }                    
                    break;
                }
                }
                // call OnEvent(SDL_Event& evnt) for all devices
                _OnEvent(&evnt);
            }

            // call OnUpdate(float fElapsedTim) for all devices
            _OnUpdate();

            // call OnRender() for all devices
            // moved to within GFX::OnUpdate() since ony sub-GFX devices are rendered
            // and only rendered by GFX::OnRender()z        
            // _OnRender(); 

            //printf("Bus::run() -- PC: $%04X\n", m_cpu->getPC());
        }

        // call OnDestroy() for all devices
        _OnDestroy();
    }

    // call OnQuit() for all devices
    _OnQuit();

    // wait for the CPU thread to close
    //SDL_WaitThread()
    th.join();
}

int Bus::getFPS() 
{    
	return s_instance->m_fps;
}

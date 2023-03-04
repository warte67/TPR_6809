/**** GfxImage.cpp ***************************************
 *
 *  Emulates the external 64k Static Ram Chip that is used for extra video ram.
 *  Handles the SDL Images for the GfxIndexed, GfxTile16, GfxTile32, and GfxSprite modes.
 *
 *  Copyright (C) 2023 by Jay Faries
 ************************************/

#include <array>
#include "types.h"
#include "MemoryMap.h"
#include "Bus.h"
#include "GFX.h"
#include "GfxMode.h"
#include "GfxImage.h"


std::array<Byte, 65536> GfxImage::s_mem_64k;
Word GfxImage::s_mem_64k_adr = 0;






GfxImage::GfxImage()
{
	printf("GfxImage::GfxImage() \n");
	bus = Bus::getInstance();
	gfx = bus->m_gfx;
}

GfxImage::~GfxImage()
{
	printf("GfxImage::~GfxImage() \n");
}


void GfxImage::OnInitialize() {
	printf("GfxImage::OnInitialize() \n"); 
}
void GfxImage::OnQuit() {
	printf("GfxImage::OnQuit() \n");
}

void GfxImage::OnCreate() {
	printf("GfxImage::OnCreate() \n");
}
void GfxImage::OnDestroy() {
	printf("GfxImage::OnDestroy() \n");
}

void GfxImage::OnActivate() {
	printf("GfxImage::OnActivate() \n");
}
void GfxImage::OnDeactivate() {
	printf("GfxImage::OnDeactivate() \n");
}

void GfxImage::OnEvent(SDL_Event* evnt) {
	//printf("GfxImage::OnEvent() \n");
}
void GfxImage::OnUpdate(float fElapsedTime) {
	//printf("GfxImage::OnUpdate() \n");
}
void GfxImage::OnRender() {
	//printf("GfxImage::OnRender() \n");
}
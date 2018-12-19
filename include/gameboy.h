#pragma once
#include <string>
#include "cpu.h"
#include "gpu.h"
#include "memorymap.h"

class Gameboy{
	public:
		Gameboy(std::string cartridgeName);
	
	private:
		MemoryMap memoryMap;
		CPU cpu;
		GPU gpu;
};
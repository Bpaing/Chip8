#include "Chip8.h"
#include "Graphics.h"
#include <iostream>
#include <chrono>
#include <string>

//main calls Cycle() until exit.
//ARG consists of:
//	Video Scale (Chip8 is only 64x32)
//	Delay/Refresh rate value for timers
//	ROM file to load
int main(int argc, char** argv)		
{
	if (argc != 4) {
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int refreshCycle = std::stoi(argv[2]);
	char const* romName = argv[3];
	
	//Texture size should correspond to original video size
	Graphics graphics("Chip-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);
	Chip8 chip8;
	chip8.LoadROM(romName);

	int pitch = sizeof(chip8.display[0]) * VIDEO_WIDTH;			//getting video pitch for SDL texture function
	auto previousCycleTime = std::chrono::high_resolution_clock::now();		//Used for delay timer
	bool quit = false;

	while (!quit) {
		quit = graphics.ProcessInput(chip8.input);

		auto currentTime = std::chrono::high_resolution_clock::now();
		//determine delay timer with current time minus last time Cycle() was called
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - previousCycleTime).count();

		if (dt > refreshCycle) {
			previousCycleTime = currentTime;
			chip8.Cycle();
			graphics.Update(chip8.display, pitch);
		}
	}

	return 0;
}
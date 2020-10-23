//By Brendan Paing. Started 10/22/2020.

#include "Chip8.h"

const unsigned int START_ADDRESS = 0x200;	//0x000 to 0x1FF is reserved, instructions start at 0x200
const unsigned int FONTSET_START = 0x050;
const unsigned int FONTSET_SIZE = 80; //(16 * 10 (A) - 16 * 5 = 80)

Chip8::Chip8()
{
	counter = START_ADDRESS;

	for (int i = FONTSET_START; i < FONTSET_SIZE, i++) {
		memory[FONTSET_START + i] = fontset[i];
	}
}

//0x050-0x0A0 requires the built-in characters (0-9, A-F)
//in binary . . . draw the characters
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0										
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
uint8_t fontset[FONTSET_SIZE]{
	0xF0, 0x90, 0x90, 0x90, 0xF0 //0
	0x20, 0x60, 0x20, 0x20, 0x70 //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0 //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0 //3
	0x90, 0x90, 0xF0, 0x10, 0x10 //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0 //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0 //6
	0xF0, 0x10, 0x20, 0x20, 0x20 //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0 //8
	0xF0, 0x90, 0xF0, 0x10, 0x10 //9
	0x60, 0x90, 0xF0, 0x90, 0x90 //A
	0xD0, 0x90, 0xD0, 0x90, 0xD0 //B
	0xF0, 0x80, 0x80, 0x80, 0xF0 //C
	0xD0, 0x90, 0x90, 0x90, 0xD0 //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0 //E
	0xF0, 0x80, 0xF0, 0x80, 0x80 //F

}
																
//copied from site austinmorlan.com
void Chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

//Below is the beginning of Chip8 instruction implementation.
//By Brendan Paing. Started 10/22/2020.

#include <cstdint>
#include <string>

class Chip8
{
	public:
		uint8_t registers[16]{};		//16 registers, each can hold 8 bits.
		uint8_t memory[4096]{};			//4096 bytes of memory, 8 bits in a byte.
		uint16_t index{};				//register that stores memory addresses. 16 bits.
		uint16_t counter{};				//register that holds the next instruction to execute in a program.
		uint16_t stack[16]{};			//stack holds 16 program counters.
		uint8_t sPtr{};					//pointer for stack management.
		uint8_t delay{};				//Timer that decrements when > 0. Default 60hz.
		uint8_t sound{};				//Similar to delay, but for sounds
		uint8_t input[16]{};			//16 inputs, all representing a hex value.
		uint32_t display[64 * 32]{};	//62 x 32 pixel display, uint32 is used for SDL later on.

		uint16_t opcode{};				//CPU instruction. uint16 is used because instructions can be specified to be hex.			


		void LoadROM(char const* filename);

		//Chip8 Instructions
		void OP_00E0();
		void OP_00EE();
		void OP_1nnn();
		void OP_2nnn();
		void OP_3xkk();
		void OP_4xkk();
		void OP_5xy0();
		void OP_6xkk();
		void OP_7xkk();
		void OP_8xy0();
		void OP_8xy1();
		void OP_8xy2();
		void OP_8xy3();
		void OP_8xy4();
		void OP_8xy5();
		void OP_8xy6();
		void OP_8xy7();
		void OP_8xyE();
		void OP_9xy0();
};

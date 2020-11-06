//By Brendan Paing. Started 10/22/2020.

#ifndef CHIP_8_H
#define CHIP_8_H

#include <cstdint>
#include <random>

const unsigned int KEY_COUNT = 16;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int REGISTER_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int STACK_SIZE = 16;


class Chip8
{
	public:
		Chip8();
		void LoadROM(char const* filename);
		void Cycle();	//Used to parse through ROM instructions.

		//These variables are public so for main and SDL2 access
		uint8_t input[KEY_COUNT]{};			//16 inputs, all representing a hex value.
		uint32_t display[VIDEO_HEIGHT * VIDEO_WIDTH]{};	//62 x 32 pixel display, uint32 is used for SDL later on.

	private:
		void Table0();	//Used to parse through sub-tables in the function pointer.
		void Table8();
		void TableE();
		void TableF();

		//Chip8 Instructions
		void OP_NULL();
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
		void OP_Annn();
		void OP_Bnnn();
		void OP_Cxkk();
		void OP_Dxyn();
		void OP_Ex9E();
		void OP_ExA1();
		void OP_Fx07();
		void OP_Fx0A();
		void OP_Fx15();
		void OP_Fx18();
		void OP_Fx1E();
		void OP_Fx29();
		void OP_Fx33();
		void OP_Fx55();
		void OP_Fx65();

		//Function Pointer Table
		//A good practice to decode opcodes is to group them based on syntax similarities.
		//These groups are then placed into function pointer arrays.
		//Study each opcode and note their similarities.
		//From austinmorlan.com :	1. Unique (12)			2. First digit repeats, unique last digit (9)
		//							3. $00E + Unique (2)	4. First digit repeats, unique last 2 digits (11)

		//Any invalid opcodes will default to OP_NULL
		//Declare typedef tables below, then store each function pointer above in constructor
		typedef void (Chip8::*Chip8Instruction) ();
		Chip8Instruction table[0xF + 1]{ &Chip8::OP_NULL };		//master table, first digit of instructions [$0, $F]
		Chip8Instruction table0[0xE + 1]{ &Chip8::OP_NULL };	//To accomodate the groupings, sub tables are created.
		Chip8Instruction table8[0xE + 1]{ &Chip8::OP_NULL };	//Sizes here are based on other digits.
		Chip8Instruction tableE[0xE + 1]{ &Chip8::OP_NULL };
		Chip8Instruction tableF[0x65 + 1]{ &Chip8::OP_NULL };

		uint8_t registers[REGISTER_COUNT]{};		//16 registers, each can hold 8 bits.
		uint8_t memory[MEMORY_SIZE]{};				//4096 bytes of memory, 8 bits in a byte.
		uint16_t index{};							//register that stores memory addresses. 16 bits.
		uint16_t counter{};							//register that holds the next instruction to execute in a program.
		uint16_t stack[STACK_SIZE]{};				//stack holds 16 program counters.
		uint8_t sPtr{};								//pointer for stack management.
		uint8_t delay{};							//Timer that decrements when > 0. Default 60hz.
		uint8_t sound{};							//Similar to delay, but for sounds
		uint16_t opcode{};							//CPU instruction. uint16 is used because instructions can be specified to be hex.

		std::default_random_engine randNumGen;				//random generator
		std::uniform_int_distribution<unsigned int> randByte;	//random number storage
};


#endif
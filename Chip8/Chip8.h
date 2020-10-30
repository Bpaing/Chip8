//By Brendan Paing. Started 10/22/2020.

#include <cstdint>
#include <string>
#include <random>
#include <chrono>

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

		std::default_random_engine randNumGen;				//random generator
		std::uniform_int_distribution<uint8_t> randByte;	//random number storage

		Chip8()		//Generally, best practice is to seed ONCE, then extract numbers.
			: randNumGen(std::chrono::system_clock::now().time_since_epoch().count()) //seed is system clock.
		{
			randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
			//grabs numbers from the generator seeded in the constructor.

			//Function pointers for instructions, patterns link to sub tables.
			table[0x0] = &Chip8::Table0;
			table[0x1] = &Chip8::OP_1nnn;
			table[0x2] = &Chip8::OP_2nnn;
			table[0x3] = &Chip8::OP_3xkk;
			table[0x4] = &Chip8::OP_4xkk;
			table[0x5] = &Chip8::OP_5xy0;
			table[0x6] = &Chip8::OP_6xkk;
			table[0x7] = &Chip8::OP_7xkk;
			table[0x8] = &Chip8::Table8;
			table[0x9] = &Chip8::OP_9xy0;
			table[0xA] = &Chip8::OP_Annn;
			table[0xB] = &Chip8::OP_Bnnn;
			table[0xC] = &Chip8::OP_Cxkk;
			table[0xD] = &Chip8::OP_Dxyn;
			table[0xE] = &Chip8::TableE;
			table[0xF] = &Chip8::TableF;

			//below are the sub-table assignments.
			table0[0x0] = &Chip8::OP_00E0;
			table0[0xE] = &Chip8::OP_00EE;

			table8[0x0] = &Chip8::OP_8xy0;
			table8[0x1] = &Chip8::OP_8xy1;
			table8[0x2] = &Chip8::OP_8xy2;
			table8[0x3] = &Chip8::OP_8xy3;
			table8[0x4] = &Chip8::OP_8xy4;
			table8[0x5] = &Chip8::OP_8xy5;
			table8[0x6] = &Chip8::OP_8xy6;
			table8[0x7] = &Chip8::OP_8xy7;
			table8[0xE] = &Chip8::OP_8xyE;

			tableE[0x1] = &Chip8::OP_ExA1;
			tableE[0xE] = &Chip8::OP_Ex9E;
			
			tableF[0x07] = &Chip8::OP_Fx07;
			tableF[0x0A] = &Chip8::OP_Fx0A;
			tableF[0x15] = &Chip8::OP_Fx15;
			tableF[0x18] = &Chip8::OP_Fx18;
			tableF[0x1E] = &Chip8::OP_Fx1E;
			tableF[0x29] = &Chip8::OP_Fx29;
			tableF[0x33] = &Chip8::OP_Fx33;
			tableF[0x55] = &Chip8::OP_Fx55;
			tableF[0x65] = &Chip8::OP_Fx65;
		}

		void LoadROM(char const* filename);

		void Cycle();	//Used to parse through ROM instructions.

		//The below functions are used to parse through the sub-tables for the correct opcode.
		void Table0()
		{
			((*this).*(table0[opcode & 0x000Fu]))();
		}

		void Table8()
		{
			((*this).*(table8[opcode & 0x000Fu]))();
		}

		void TableE()
		{
			((*this).*(tableE[opcode & 0x000Fu]))();
		}

		void TableF()
		{
			((*this).*(tableF[opcode & 0x00FFu]))();
		}

		void OP_NULL()	 //Dummy function, called if an invalid opcode passes through.
		{}

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



};

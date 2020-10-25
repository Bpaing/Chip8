//By Brendan Paing. Started 10/22/2020.

#include "Chip8.h"

const unsigned int START_ADDRESS = 0x200;	//0x000 to 0x1FF is reserved, instructions start at 0x200
const unsigned int FONTSET_START = 0x050;	//0x050-0x0A0
const unsigned int FONTSET_SIZE = 80; //(16 * 10 (A) - 16 * 5 = 80)

Chip8::Chip8()
{
	counter = START_ADDRESS;

	for (int i = FONTSET_START; i < FONTSET_SIZE; i++) {
		memory[FONTSET_START + i] = fontset[i];
	}
}

//0x050-0x0A0 requires the built-in characters (0-9, A-F)
//in binary . . . draw the characters, convert to hex
//Fontset size is 80, and there are 16 characters. Each character gets 5 indices.
uint8_t fontset[FONTSET_SIZE]{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x20, 0x20, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0x10, //9
	0x60, 0x90, 0xF0, 0x90, 0x90, //A
	0xD0, 0x90, 0xD0, 0x90, 0xD0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xD0, 0x90, 0x90, 0x90, 0xD0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80, //F
};

/*
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
}*/

//Instruction implementation
void Chip8::OP_00E0()	//CLS; clear display
{
	std::memset(display, 0, sizeof(display));
}

void Chip8::OP_00EE()	//RET; return subroutine;	counter to address at top of stack, stack pointer - 1
{
	--sPtr;
	counter = stack[sPtr];
}

void Chip8::OP_1nnn()	//JP; set program counter to address nnn
{
	uint16_t address = opcode & 0xFFFu;		//After 0xFFF (end of ROM instruction), free space to use		010100100 &
	counter = address;						//The & bitwise operator is like && but for smaller data.		101101100
}											//																000100100

void Chip8::OP_2nnn()	//CALL;	subroutine at nnn;	Increment stack pointer, current counter on top of stack, then set to nnn.
{												//0x0FFFu	 F represents the digits we want to grab.
	uint16_t address = opcode & 0x0FFFu;		//When we use CALL, we want to increment the stack such that PC doesn't return to CALL.
	++sPtr;										
	stack[sPtr] = counter;						//So we increment to the next instruction so that PC doesn't return to CALL.
	counter = address;
}

void Chip8::OP_3xkk()	//SE Vx, byte;	If Vx = kk, skip next instruction.
{											// If register Vx = kk, increment PC by 2.
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;	//Shift F to the right by 8 bits since register only goes up to 16.
	uint8_t byte = opcode & 0x00FFu;		//Take xxxx format opcode, grab kk which is the far right two.		

	if (registers[Vx] == byte) {
			counter += 2;
	}
}

void Chip8::OP_4xkk()	//SNE Vx, byte;	If Vx != kk, skip next instruction.
{											//3xkk but "not equals".
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;	
	uint8_t byte = opcode & 0x00FFu;		

	if (registers[Vx] != byte) {
		counter += 2;
	}
}

void Chip8::OP_5xy0()	//SE Vx, Vy; If registers Vx == Vy, skip next instruction.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;	//Grab Vx and Vy, shift both to rightmost.
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] -= registers[Vy]) {
		counter += 2;
	}
}

void Chip8::OP_6xkk()	//LD Vx, byte;	Put value kk into register Vx.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

void Chip8::OP_7xkk()	//ADD Vx, byte; Add value kk to value at register Vx, store at register Vx.
{
	uint8_t Vx = (opcode && 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

void Chip8::OP_8xy0()	//LD Vx, Vy; Set value of register Vy in register Vx
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()	//OR Vx, Vy; Bitwise OR operation on Vx and Vy, store in Vx.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()	//OR Vx, Vy; Bitwise AND operation on Vx and Vy, store in Vx.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()	//XOR Vx, Vy; Bitwise OR exclusive operation on Vx and Vy, store in Vx.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()	//ADD Vx, Vy; add values of Vx and Vy, store in Vx, left over bits indicated by register VF.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;		//VF . . . last register
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;		//Carry over if result > 8 bits (255).								
	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255u) {	
		registers[0xFu] = 1;
	} else {
		registers[0xFu] = 0;
	}

	registers[Vx] = sum & 0xFFu;				//Use bitwise AND to reduce 16 bit sum to 8 bit register value.
}

void Chip8::OP_8xy5()	//SUB Vx, Vy; If Vx > Vy, VF = 1, else 0. Then Vx - Vy value stored in register Vx.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;		//VF . . . last register
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;		//Carry over if result > 8 bits (255).

	if (registers[Vx] > registers[Vy]) {
		registers[0xFu] = 1;
	} else {
		registers[0xFu] = 0;
	}

	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()	//SHR Vx{, Vy}; if least-significant digit of Vx = 1, then VF = 1. Then Vx / 2.
{						//Least significant digit = rightmost digit.
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[0xFu] = registers[Vx] & 0x1u;		//0x1u == 0x01u, so checking least-sig
	registers[Vx] >>= 1;						//Dividing bits is basically just shifting. '=' indiciates unsigned.
}

void Chip8::OP_8xy7()	//SUBN Vx, Vy; Vx = Vy - Vx, If Vy > Vx, VF = 1, else 0.
{						//8xy5 but Vx and Vy are flipped.
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx]) {
		registers[0xFu] = 1;
	}
	else {
		registers[0xFu] = 0;
	}

	registers[Vx] = (registers[Vy] - registers[Vx]);
}

void Chip8::OP_8xyE()	//SHL Vx{ , Vy }; If most-significant digit of Vx = 1, then VF = 1. Then Vx * 2;
{						
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;	//0x80 in binary is 1000000, thus the MSB.
	registers[Vx] <<= 1;							//then we shift it 7 right to store in VF as a single digit.
}

void Chip8::OP_9xy0()	//SNE Vx, Vy; If Vx != Vy, skip next instruction.
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy]) {
		counter += 2;
	}
}


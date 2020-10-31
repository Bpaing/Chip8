//By Brendan Paing. Started 10/22/2020.

#include "Chip8.h"

const unsigned int START_ADDRESS = 0x200;	//0x000 to 0x1FF is reserved, instructions start at 0x200
const unsigned int FONTSET_START = 0x050;	//0x050-0x0A0
const unsigned int FONTSET_SIZE = 80; //(16 * 10 (A) - 16 * 5 = 80)

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

Chip8::Chip8()	//Generally, best practice is to seed ONCE, then extract numbers.
	: randNumGen(std::chrono::system_clock::now().time_since_epoch().count()) //seed is system clock.
{
	counter = START_ADDRESS;

	for (int i = FONTSET_START; i < FONTSET_SIZE; i++) {
		memory[FONTSET_START + i] = fontset[i];
	}

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

//Fetch opcode from instructions
//Decode which opcode to use
//Execute opcode
void Chip8::Cycle()
{
	//Fetch		//memory is 0x00, while opcodes are 0x0000. Shift left then add next to get full opcode
	opcode = (memory[counter] << 8u) | memory[counter + 1];

	//Increment counter to the next opcode before executing
	counter += 2;

	//Decode and Execute 
	//Determine which group the opcode belongs in using first digit
	//Then, shift to rightmost digit to access master table indices (0 - F).
	//From there, function pointer does it
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	//Decrement the delay timer if it's been set
	if (delay > 0)
	{
		--delay;
	}

	//Decrement the sound timer if it's been set
	if (sound > 0)
	{
		--sound;
	}
}


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
	uint16_t address = opcode & 0x0FFFu;	//After 0xFFF (end of ROM instruction), free space to use		010100100 &
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

void Chip8::OP_Annn()	//LD I;	Index register set to nnn.
{
	uint16_t address = opcode & 0x0FFFu;
	index = address;
}

void Chip8::OP_Bnnn()	//JP V0; counter set to nnn + value of V0
{
	uint16_t address = opcode & 0x0FFFu;
	counter = address + registers[0x0u];
}

void Chip8::OP_Cxkk()	//RND Vx, byte; set value of register Vx to random byte AND kk
{						//Seed RNG in the constructor, then initialize with a instance member
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t kk = (opcode & 0x00FFu);

	registers[Vx] = randByte(randNumGen) & kk;
}

//Read n-byte starting at memory location I, displayed as sprites at coordinate (Vx, Vy)
//Sprites are XOR'd onto the screen (in case there are other sprites present).
//If any sprites are deleted as a result of XOR, VF = 1, otherwise 0;
//Any sprites on the edges should wrap around.
void Chip8::OP_Dxyn()	//DRW Vx, Vy, nibble;
{						
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t nBytes = opcode & 0x000Fu;

	uint8_t posX = registers[Vx] & VIDEO_WIDTH;	//video and screen will be visited again when we do graphics.
	uint8_t posY = registers[Vy] & VIDEO_HEIGHT;

	for (unsigned int row = 0; row < nBytes; row++) {
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; col++) {	//8 bit int = 8 columns.
			//0x8u is the leftmost digit = 1. Rightshift by col to set each column to '1' (on).
			uint8_t spritePixel = spriteByte & (0x8u >> col);
			uint32_t* screenPixel = &display[(posY + row) * VIDEO_WIDTH + (posX + col)];

			if (spritePixel) {	//if the sprite pixel is on, check for collision
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_NULL()	//Dummy function, called if an invalid opcode passes through.
{}

void Chip8::OP_Ex9E()	//SKP Vx; Skip next instruction if key pressed = value at Vx.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t key = registers[Vx];
	if (input[key]) {
		counter += 2;
	}
}

void Chip8::OP_ExA1()	//SKNP Vx; Ex9E, but NOT pressed.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t key = registers[Vx];
	if (!input[key]) {
		counter += 2;
	}
}

void Chip8::OP_Fx07()	//LD Vx, DT; Set Vx to DT (delay timer)
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registers[Vx] = delay;
}

void Chip8::OP_Fx0A()	//LD Vx, K; Store the next pressed key value K in Vx.
{											//Stops all execution until pressed. We 'wait' by decrementing counter by 2 if nothing pressed.
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;	//This is basically skipping but in reverse, always coming back to the same instruction.

	bool inputCheck = false;
	for (int i = 0; !inputCheck & i < 16; i++) {
		if (input[i]) {
			registers[Vx] = i;
			inputCheck = true;
		}
	}
	if (!inputCheck) {
		counter -= 2;
	}
}

void Chip8::OP_Fx15()	//LD DT, Vx; Set DT (delay timer) to Vx.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	delay = registers[Vx];
}

void Chip8::OP_Fx18()	//LD ST, Vx; Set ST (sound timer) to Vx.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	sound = registers[Vx];
}

void Chip8::OP_Fx1E()	//ADD I, Vx; Set I = I + Vx
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	index += registers[Vx];
}

void Chip8::OP_Fx29()	//LD F, Vx; Value of I set to hex sprite corresponding to Vx.
{						//Sprite in this case means fontset. Each 'sprite' is 5 bytes.
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t value = registers[Vx];
	index = FONTSET_START + (5 * value); //FONTSET_START = 0x50, (5 * value) gets us the start of each char.
}

void Chip8::OP_Fx33()	//LD B, Vx; Decimal Value of Vx. Hundreds in I, Tens in I + 1, Ones in I + 2.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t value = registers[Vx];

	for (int i = 0; i < 3; i++) {
		memory[index + (2 - i)] = value % 10;
		value /= 10;
	}
}

void Chip8::OP_Fx55()	//LD[I], Vx; Store registers V0 to Vx, starting at I.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;

	for (int i = 0; i <= Vx; i++) {
		memory[index + i] = registers[i];
	}
}

void Chip8::OP_Fx65()	//LD Vx, [I]; Fx55, but read from I, store into registers V0 to Vx.
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;

	for (int i = 0; i <= Vx; i++) {
		registers[i] = memory[index + i];
	}
}

//The below functions are used to parse through the sub-tables for the correct opcode.
void Chip8::Table0()
{
	((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode & 0x00FFu]))();
}
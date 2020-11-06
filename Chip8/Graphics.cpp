#include "Graphics.h"
#include <SDL.h>

//Initialize our SDL instances (window, renderer, texture)
Graphics::Graphics(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	SDL_Init(SDL_INIT_VIDEO);	//This function must ALWAYS be called to use SDL.
								//Parameters are subsystems like video, audio, etc.

	window = SDL_CreateWindow(title, 200, 200, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//param format: lots of different formats, experiment and see what is compatible/fastest! getRendererInfo()
	//param access: should the texture be accessible (is it static or does it change constantly?)
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);

}

Graphics::~Graphics() //Destroy all the SDL elements then exit.
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_Quit();
}

void Graphics::Update(void const* buffer, int pitch)
{
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);		//buffer is raw pixel data, pitch # bytes per row of data
	SDL_RenderClear(renderer);								//clear the current render
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);	//insert the new texture
	SDL_RenderPresent(renderer);							//make new texture visible
}


//	Original Keypad Input
//	+ - + - + - + - +
//	| 1 | 2 | 3 | C |
//	+ - + - + - + - +
//	| 4 | 5 | 6 | D |
//	+ - + - + - + - +
//	| 7 | 8 | 9 | E |    
//	+ - + - + - + - +
//	| A | 0 | B | F |
//	+ - + - + - + - +
bool Graphics::ProcessInput(uint8_t* keys)
{
	bool quit = false;
	SDL_Event event;

	while (SDL_PollEvent(&event))	//loop entered if a pending event exists (input, quit, etc.)
	{
		//now we have to account for all the possible inputs on the emulator.
		//There are 3 types to consider here: QUIT, KEYDOWN (press), KEYUP (release)
		switch (event.type)
		{
			case SDL_QUIT:
			{
				quit = true;
			} break;

			case SDL_KEYDOWN:	 //if a key is pressed, then it's corresponding value should be 1.
			{					 //Here, we'll bind the keys according to qwerty.
				switch (event.key.keysym.sym) //event -> key event -> corresponding key -> key's code in SDL
				{
					case SDLK_ESCAPE:		//let's use esc to quit the program as well.
					{
						quit = true;
					} break;

					case SDLK_x:
					{
						keys[0] = 1;
					} break;

					case SDLK_1:
					{
						keys[1] = 1;
					} break;

					case SDLK_2:
					{
						keys[2] = 1;
					} break;

					case SDLK_3:
					{
						keys[3] = 1;
					} break;

					case SDLK_q:
					{
						keys[4] = 1;
					} break;

					case SDLK_w:
					{
						keys[5] = 1;
					} break;

					case SDLK_e:
					{
						keys[6] = 1;
					} break;

					case SDLK_a:
					{
						keys[7] = 1;
					} break;

					case SDLK_s:
					{
						keys[8] = 1;
					} break;

					case SDLK_d:
					{
						keys[9] = 1;
					} break;

					case SDLK_z:
					{
						keys[0xA] = 1;
					} break;

					case SDLK_c:
					{
						keys[0xB] = 1;
					} break;

					case SDLK_4:
					{
						keys[0xC] = 1;
					} break;

					case SDLK_r:
					{
						keys[0xD] = 1;
					} break;

					case SDLK_f:
					{
						keys[0xE] = 1;
					} break;

					case SDLK_v:
					{
						keys[0xF] = 1;
					} break;
				}
			} break;

			case SDL_KEYUP:	 //if a key is released then it's corresponding value should be 0.
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_x:
					{
						keys[0] = 0;
					} break;

					case SDLK_1:
					{
						keys[1] = 0;
					} break;

					case SDLK_2:
					{
						keys[2] = 0;
					} break;

					case SDLK_3:
					{
						keys[3] = 0;
					} break;

					case SDLK_q:
					{
						keys[4] = 0;
					} break;

					case SDLK_w:
					{
						keys[5] = 0;
					} break;

					case SDLK_e:
					{
						keys[6] = 0;
					} break;

					case SDLK_a:
					{
						keys[7] = 0;
					} break;

					case SDLK_s:
					{
						keys[8] = 0;
					} break;

					case SDLK_d:
					{
						keys[9] = 0;
					} break;

					case SDLK_z:
					{
						keys[0xA] = 0;
					} break;

					case SDLK_c:
					{
						keys[0xB] = 0;
					} break;

					case SDLK_4:
					{
						keys[0xC] = 0;
					} break;

					case SDLK_r:
					{
						keys[0xD] = 0;
					} break;

					case SDLK_f:
					{
						keys[0xE] = 0;
					} break;

					case SDLK_v:
					{
						keys[0xF] = 0;
					} break;
				}
			} break;
		} //end of switch case
	} //end of while loop
	return quit;
}

#include "Graphics.h"
#include <SDL.h>

//Initialize our SDL instances (window, renderer, texture)
Graphics::Graphics(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	SDL_Init(SDL_INIT_VIDEO);	//This function must ALWAYS be called to use SDL.
								//Parameters are subsystems like video, audio, etc.

	window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);

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
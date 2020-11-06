//Chip8 provides the framework for the emulator, but now we need a way to see it.
//Uses SDL library for rendering.

#include <cstdint>

class SDL_Window;		//Window for our emulator		
class SDL_Renderer;		//GPU can now be used to render 
class SDL_Texture;		//Can now map sprites onto the screen

class Graphics {
	public:
		Graphics(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
		~Graphics();
		void Update(void const* buffer, int pitch);
		bool ProcessInput(uint8_t* keys);

	private:
		SDL_Window* window{};
		SDL_Renderer* renderer{};
		SDL_Texture* texture{};
};

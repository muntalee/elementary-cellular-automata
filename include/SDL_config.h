#ifndef SDL_CONFIG_H
#define SDL_CONFIG_H

#include <SDL2/SDL.h>

#define FONT_SIZE       18
#define FONT_PATH       "assets/Arial.ttf"

// SDL functions
int initializeSDL(void);
SDL_Window* createWindow(const char* title, int width, int height);
SDL_Renderer* createRenderer(SDL_Window* window);
void handleEvents(int* quit);
void cleanup(SDL_Renderer* renderer, SDL_Window* window);
void initializeFont(void);
void cleanupFont(void);

#endif // SDL_CONFIG_H

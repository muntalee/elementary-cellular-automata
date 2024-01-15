#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <SDL2/SDL.h>

// SDL Rendering for Simulation
#define CELL_SIZE       5
#define SCREEN_WIDTH    410
#define SCREEN_HEIGHT   410
#define NUM_CELLS       SCREEN_WIDTH / CELL_SIZE

// simulation functions
int calculateState(int ruleset, int left, int curr, int right);
void getNextGeneration(int* cells, int ruleset);
void drawGeneration(int* cells, int y, SDL_Renderer *renderer);

#endif // AUTOMATA_H

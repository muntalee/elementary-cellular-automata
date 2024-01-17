#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <SDL2/SDL.h>

// SDL Rendering for Simulation
#define CELL_SIZE       10
#define SCREEN_WIDTH    810
#define SCREEN_HEIGHT   610
#define NUM_CELLS       SCREEN_WIDTH / CELL_SIZE

// simulation functions
int calculateState(int ruleset, int left, int curr, int right);
void getNextGeneration(int* cells, int ruleset);
void drawGeneration(int* cells, int y);

#endif // AUTOMATA_H

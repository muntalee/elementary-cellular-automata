#ifndef AUTOMATA_H
#define AUTOMATA_H

// SDL Rendering for Simulation
#define SCREEN_WIDTH    810
#define SCREEN_HEIGHT   610

// simulation functions
int calculateState(int left, int curr, int right);
void getNextGeneration(void);
void drawGeneration(int y);
void renderAutomata(void);

#endif // AUTOMATA_H

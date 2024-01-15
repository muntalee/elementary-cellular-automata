#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>

#include "SDL_config.h"
#include "automata.h"

SDL_Window *window;
SDL_Renderer *renderer;

int main(void) {

    // ensures SDL is working
    if (initializeSDL() != 0)
        return 1;

    // create window
    window = createWindow("Elementary Cellular Automata", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!window)
        return 1;

    // create renderer
    renderer = createRenderer(window);
    if (!renderer)
        return 1;

    // color in white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // clear renderer and display it
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // SETUP SIMULATION
    int i, y, ruleset;

    ruleset = 30; /* rules for the cells to operate with */

    int *cells = (int* )malloc(NUM_CELLS * sizeof(int));
    for (i = 0; i < NUM_CELLS; i++) cells[i] = 0;
    cells[NUM_CELLS/2] = 1;

    // generate the simulation
    y = 0;
    while (y < SCREEN_WIDTH) {
        drawGeneration(cells, y, renderer);
        getNextGeneration(cells, ruleset);
        y += CELL_SIZE;
    }

    SDL_RenderPresent(renderer);


    // window loop
    int quit = 0;
    while (!quit) {
        // Input handling
        handleEvents(&quit);
    }

    // cleanup sdl resources
    free(cells);
    cleanup(renderer, window);

    return 0;
}

/*
 * Function:  calculateState
 * --------------------
 * Finds state between current, left, and right neighbors
 *
 *  ruleset:    decimal value indicating the rules
 *  left:       left neighbor
 *  curr:       current neightbor
 *  right:      right neighbor
 *
 *  returns: integer representing the new state of curr
 */
int calculateState(int ruleset, int left, int curr, int right) {
    int value, pos, dec;
    int rules[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    value = 7 - ((left*4) + (curr*2) + right); /* used for position */
    pos = 7;
    dec = ruleset;

    // fill array with value
    while (dec != 0) {
        rules[pos] = dec % 2;
        dec /= 2;
        pos--;
    }

    return rules[value];
}

/*
 * Function:  getNextGeneration
 * --------------------
 * changes cells into the next generation, using a ruleset number
 *
 *  cells:      array of ints representing the cells
 *  ruleset:    decimal value indicating the rules
 */
void getNextGeneration(int* cells, int ruleset) {
    int i;
    int *newCells = (int* )malloc(NUM_CELLS * sizeof(int));
    for (i = 1; i < NUM_CELLS-1; i++) {
        // calculate the state
        newCells[i] = calculateState(ruleset, cells[i - 1], cells[i], cells[i + 1]);
    }

    // include wrap around
    newCells[0] = calculateState(ruleset, cells[NUM_CELLS-1], cells[0], cells[1]);
    newCells[NUM_CELLS - 1] = calculateState(ruleset, cells[NUM_CELLS - 2],
            cells[NUM_CELLS - 1], cells[0]);

    // replace old cells to new cells
    for (i = 0; i < NUM_CELLS; i++) cells[i] = newCells[i];
    free(newCells);
}

/*
 * Function:  drawGeneration
 * --------------------
 * draws cells into the screen
 *
 *  cells:      array of ints representing the cells
 *  row:        row (in pixels) for the cells to occupy
 *  renderer:   the SDL renderer to destroy
 *
 */
void drawGeneration(int* cells, int row, SDL_Renderer *renderer) {
    int i;
    for (i = 0; i < NUM_CELLS; i++) {
        SDL_Rect rect;
        rect.x = i * CELL_SIZE;
        rect.y = row;
        rect.w = CELL_SIZE;
        rect.h = CELL_SIZE;

        int color = 255 - (255 * cells[i]);

        SDL_SetRenderDrawColor(renderer, color, color, color, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}


/*
 * Function:  initializeSDL
 * --------------------
 * Initializes the SDL library for video.
 *
 *  returns: 0 on success, 1 on failure
 */
int initializeSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}

/*
 * Function:  createWindow
 * --------------------
 * Creates an SDL window with the specified title, width, and height.
 *
 *  title:   the title of the window
 *  width:   the width of the window
 *  height:  the height of the window
 *
 *  returns: a pointer to the created SDL window, or NULL on failure
 */
SDL_Window *createWindow(const char *title, int width, int height) {
    SDL_Window *window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }
    return window;
}

/*
 * Function:  createRenderer
 * --------------------
 * Creates an SDL renderer for the given window.
 *
 *  window:  the SDL window to create the renderer for
 *
 *  returns: a pointer to the created SDL renderer, or NULL on failure
 */
SDL_Renderer *createRenderer(SDL_Window *window) {
    SDL_Renderer *renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    return renderer;
}

/*
 * Function:  handleEvents
 * --------------------
 * Handles SDL events, specifically quitting when the close button is clicked.
 *
 *  quit:    a pointer to a flag indicating whether the program should quit
 */
void handleEvents(int *quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // on clicking close, quit
        if (event.type == SDL_QUIT) {
            *quit = 1;
        }
    }
}

/*
 * Function:  cleanup
 * --------------------
 * Cleans up SDL resources, destroying the renderer and window, and quitting
 * SDL.
 *
 *  renderer:  the SDL renderer to destroy
 *  window:    the SDL window to destroy
 */
void cleanup(SDL_Renderer *renderer, SDL_Window *window) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

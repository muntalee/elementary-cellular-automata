#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

#include "automata.h"
#include "renderer.h"
#include "microui.h"

mu_Context *ctx;

// background color
static  float bg[3] = { 255, 255, 255 };
static   char ruleStr[4] = "30";
static   char cellSizeStr[4] = "5";
static    int *cells;

// initial values for cellular automata
static    int ruleset = 30;
static    int CELL_SIZE = 5;
static    int NUM_CELLS;

// sample ui window
static void settings_window(mu_Context *ctx) {
    if (mu_begin_window(ctx, "Configure", mu_rect(10, 10, 145, 105))) {
        mu_layout_row(ctx, 2, (int[]) { 60, -1 }, 0);

        mu_label(ctx, "Ruleset");
        mu_textbox(ctx, ruleStr, sizeof(ruleStr));

        mu_label(ctx, "Cell Size");
        mu_textbox(ctx, cellSizeStr, sizeof(cellSizeStr));

        if (mu_button(ctx, "Render")) {
            ruleset = (atoi(ruleStr) == 0) ? ruleset : atoi(ruleStr);
            CELL_SIZE = (atoi(cellSizeStr) == 0) ? CELL_SIZE : atoi(cellSizeStr);
            NUM_CELLS = SCREEN_WIDTH / CELL_SIZE;
        }
        mu_end_window(ctx);
    }
}

// processing frame for gui
static void process_frame(mu_Context *ctx) {
    mu_begin(ctx);
    settings_window(ctx);
    mu_end(ctx);
}

// button mappings
static const char button_map[256] = {
    [ SDL_BUTTON_LEFT   & 0xff ] =  MU_MOUSE_LEFT,
    [ SDL_BUTTON_RIGHT  & 0xff ] =  MU_MOUSE_RIGHT,
    [ SDL_BUTTON_MIDDLE & 0xff ] =  MU_MOUSE_MIDDLE,
};

// key maps
static const char key_map[256] = {
    [ SDLK_LSHIFT       & 0xff ] = MU_KEY_SHIFT,
    [ SDLK_RSHIFT       & 0xff ] = MU_KEY_SHIFT,
    [ SDLK_LCTRL        & 0xff ] = MU_KEY_CTRL,
    [ SDLK_RCTRL        & 0xff ] = MU_KEY_CTRL,
    [ SDLK_LALT         & 0xff ] = MU_KEY_ALT,
    [ SDLK_RALT         & 0xff ] = MU_KEY_ALT,
    [ SDLK_RETURN       & 0xff ] = MU_KEY_RETURN,
    [ SDLK_BACKSPACE    & 0xff ] = MU_KEY_BACKSPACE,
};


// get text width for gui
static int text_width(mu_Font font, const char *text, int len) {
    if (len == -1) { len = strlen(text); }
    return r_get_text_width(text, len);
}

// get text height for gui
static int text_height(mu_Font font) {
    return r_get_text_height();
}


/* -------------
 *
 * MAIN FUNCTION
 *
 * -------------
 * */

int main(void) {

    // SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    r_init();

    // microui
    ctx = malloc(sizeof(mu_Context));
    mu_init(ctx);
    ctx->text_width = text_width;
    ctx->text_height = text_height;

    // initial cells
    NUM_CELLS = SCREEN_WIDTH / CELL_SIZE;
    cells = (int *)malloc(NUM_CELLS * sizeof(int));
    for (int i = 0; i < NUM_CELLS; i++) cells[i] = 0;

    // Main loop
    for (;;) {
        // Input handling
        handleEvents();
        process_frame(ctx);

        // gui rendering
        r_clear(mu_color(bg[0], bg[1], bg[2], 255));
        renderAutomata();

        mu_Command *cmd = NULL;
        while (mu_next_command(ctx, &cmd)) {
            switch (cmd->type) {
                case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
                case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
                case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
                case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
            }
        }

        r_present();
    }

    free(cells);
    free(ctx);
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
int calculateState(int left, int curr, int right) {
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
 */
void getNextGeneration(void) {
    int i;
    int *newCells = (int* )malloc(NUM_CELLS * sizeof(int));
    for (i = 1; i < NUM_CELLS-1; i++) {
        // calculate the state
        newCells[i] = calculateState(cells[i - 1], cells[i], cells[i + 1]);
    }

    // include wrap around
    newCells[0] = calculateState(cells[NUM_CELLS-1], cells[0], cells[1]);
    newCells[NUM_CELLS - 1] = calculateState(cells[NUM_CELLS - 2],
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
 *  row:        row (in pixels) for the cells to occupy
 *
 */
void drawGeneration(int row) {
    int i;
    for (i = 0; i < NUM_CELLS; i++) {
        int color = 255 - (255 * cells[i]);
        r_draw_rect(mu_rect(i * CELL_SIZE, row, CELL_SIZE, CELL_SIZE),
                    mu_color(color, color, color, 255));
    }
}

/*
 * Function:  renderAutomata
 * --------------------
 * Handles rendering the pattern
 *
 */
void renderAutomata(void) {
    int i;
    cells = (int *)malloc(NUM_CELLS * sizeof(int));
    for (i = 0; i < NUM_CELLS; i++)
        cells[i] = 0;
    cells[NUM_CELLS / 2] = 1;

    int y = 0;
    while (y < SCREEN_HEIGHT) {
        drawGeneration(y);
        getNextGeneration();
        y += CELL_SIZE;
    }

}

/*
 * Function:  handleEvents
 * --------------------
 * Handles input events
 *
 */
void handleEvents(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS); break;
            case SDL_MOUSEMOTION: mu_input_mousemove(ctx, event.motion.x, event.motion.y); break;
            case SDL_MOUSEWHEEL: mu_input_scroll(ctx, 0, event.wheel.y * -30); break;
            case SDL_TEXTINPUT: mu_input_text(ctx, event.text.text); break;
            
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                int b = button_map[event.button.button & 0xff];
                if (b && event.type == SDL_MOUSEBUTTONDOWN)
                    mu_input_mousedown(ctx, event.button.x, event.button.y, b);
                if (b && event.type ==   SDL_MOUSEBUTTONUP)
                    mu_input_mouseup(ctx, event.button.x, event.button.y, b);
                break;
            }
            
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                int c = key_map[event.key.keysym.sym & 0xff];
                if (c && event.type == SDL_KEYDOWN) mu_input_keydown(ctx, c);
                if (c && event.type ==   SDL_KEYUP) mu_input_keyup(ctx, c);
                break;
            }
        }
    }
}

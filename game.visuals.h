#ifndef game_visuals_h
#define game_visuals_h

#include "game.h"

#define ESC "\033[" 

#define move_cursor_up(x) ESC x "A" 
#define move_cursor_down(x) ESC x "B" 
#define move_cursor_forward(x) ESC x "C" 
#define move_cursor_backward(x) ESC x "D" 

#define erase ESC "K"

enum colors {
	green_foreground, blue_foreground, red_foreground,
	green_background, blue_background, red_background, white_background,
	reset,
	colors_number
};


enum cell_types process_move_visualized(struct game *, unsigned move);
void process_moves(struct game *, unsigned *moves, unsigned size);

void start(struct game*);
void end(struct game*);
int print_score(struct game*, enum colors color);

void print_grid(int height, int width);

void color_cell(int column, int line, enum colors color);
void color_cells(struct field *, unsigned *cells, unsigned size, enum colors color);

#endif

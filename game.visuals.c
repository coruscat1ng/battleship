#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "game.visuals.h"

const char *colors[colors_number] = {
	[green_foreground] = ESC "92m",
	[red_foreground] = ESC "91m",
	[blue_foreground] = ESC "94m",

	[green_background] = ESC "102m",
	[red_background] = ESC "101m",
	[blue_background] = ESC "104m",
	[white_background] = ESC "107m",
 	
	[reset] = ESC "0m"	
};

const enum colors cell_colors[cell_types_number] = {
	[empty] = blue_background,
	[hit] = red_background,
	[unknown] = reset,
	[ship] = green_background
};

enum cell_types process_move_visualized(struct game *game, unsigned move)
{
	
	enum cell_types cell_type = process_move(game, move);
	int line, column;
	line = move / game -> field -> width;
	column = 1 + move - game -> field -> width * line;
	line++;

	color_cell(column, line, cell_colors[cell_type]);
	printf(move_cursor_up("1"));
	print_score(game, green_foreground);
	fflush(stdout);
	return cell_type;
}

void process_moves(struct game *game, unsigned int *moves, unsigned int size)
{
	int delay = (2 << 20) / size;
	
	for(int move_number = 0; game -> field -> remained_ships && move_number < size; move_number++) {
		process_move_visualized(game, moves[move_number]);
		fflush(stdout);
		usleep(delay);
	}
}


void start(struct game *game) 
{
	printf("%sG A M E %s%s\n\n", colors[green_background], game -> name, colors[reset]);
	print_grid(game -> field -> width, game -> field -> height);
	printf(move_cursor_up("%d"), (game -> field -> height << 1) + 1);
	fflush(stdout);
	
	if(game -> field -> ships)
		color_cells(game -> field, game -> field -> ships, game -> field -> remained_ships, green_background);
}

void end(struct game* game)
{
	if(!game -> field -> remained_ships) {
		unsigned remained_moves_number = (game -> field -> height) * (game -> field -> width) - game -> move_number;
		game -> score += remained_moves_number; 
	}
	printf(move_cursor_up("2") "%sG A M E %s finished%s\n", colors[red_background], game -> name, colors[reset]);
	print_score(game, red_foreground);
	printf(move_cursor_down("%d") erase, (game -> field -> height << 1) + 1);
}

int print_score(struct game *game, enum colors color)
{
  return printf("%sScore: %s%2d %sMoves: %s%2d %sRemained Ships: %s%2d\n",
			colors[color], colors[reset], game -> score,
			colors[color], colors[reset], game -> move_number,
			colors[color], colors[reset], game -> field -> remained_ships);
	
}


void color_cells(struct field *field, unsigned *cells, unsigned size, enum colors color) 
{
	int delay = (2 << 18) / size;
	int column, line;
	for(int i = 0; i < size; i++) {
		line = cells[i] / field -> width;
		column = 1 + cells[i] - field -> width * line;
		line++;
		color_cell(column, line, color);	
		fflush(stdout);
		usleep(delay);
	}
}

void color_cell(int column, int line, enum colors color) {
	for(int j = 1; j < column; j++) {
		printf("⋅---");
	}

	line = (line << 1) - 1;
	column = (1 + column) << 2;

	printf(move_cursor_down("%d") "|%s   %s", line, colors[color], colors[reset]);
	printf(move_cursor_backward("%d") move_cursor_up("%d"), column, line);
}

void print_grid(int width, int height) 
{
	for(int i = 0; i < width; i++) {
		printf("⋅---");
	}
	printf("⋅\n");

	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++) {
			printf("|   ");
		}
		printf("|\n");
		
		for(int i = 0; i < width; i++) {
			printf("⋅---");
		}
		printf("⋅\n");
	}
}



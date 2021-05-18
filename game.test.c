#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


#include "game.visuals.h"

int test0(void);
int test(void);

int main(void) 
{
	return  test0();
}

int test0(void)
{
	struct field test_field = {
		.height = 5,
		.width = 7,
		.remained_ships = 29,
		.ships = 0
	};
	
	print_grid(test_field.width, test_field.height);
	printf(move_cursor_up("%d"), 1 + (test_field.height << 1));
	color_cell(1, 1, green_background);
	printf(move_cursor_down("%d"), 1 + (test_field.height << 1));

	test();	
	return 0;
}

int test(void) 
{
	srand(time(0));

	unsigned ships[5];
	struct field test_field = {
		.height = 5,
		.width = 8,
		.remained_ships = 5,
		.ships = ships
	};
	struct game test_game = {
		.name = "asdas",
		.move_number = 0,
		.field = &test_field 
	};
  
	unsigned cells_number = test_game.field -> height * test_game.field -> width;

 	fill_set(test_game.field -> ships, test_game.field -> remained_ships, cells_number);

	unsigned moves[test_game.field -> height * test_game.field -> width];
	fill_set(moves, cells_number, cells_number);
	
	start(&test_game);
	
	process_moves(&test_game, moves, cells_number);	
	
	
	end(&test_game);	
	
	return 0;	
}



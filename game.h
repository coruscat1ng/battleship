#ifndef game_h
#define game_h



enum cell_types {
	empty,
	ship,
	hit,
	unknown,
	cell_types_number
};

struct game {
	char name[7];
	int score;
	int streak;
	unsigned move_number;
		
	struct field {
		unsigned height;
		unsigned width;
		unsigned remained_ships;
		unsigned *ships;
	} *field;
};

void fill_set(unsigned* set, unsigned size, unsigned range);

enum cell_types process_move(struct game *, unsigned int move);




#endif

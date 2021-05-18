#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "game.h"

enum cell_types process_move(struct game *game, unsigned int move)
{
	enum cell_types cell_type = empty;
	
	for(int j = 0; j < game -> field -> remained_ships; j++) {
		if(game -> field -> ships[j] == move) {
			(game -> field -> remained_ships)--;
			game -> field -> ships[j] = game -> field -> ships[game -> field ->  remained_ships];
			cell_type = hit;
			break;
		}
	}
	
	switch (cell_type) {
		case hit:
			game -> score += (1 + game -> streak) << 2;
			game -> streak++;
			break;
		default:
			game -> streak = 0;
			break;
	}	
	
  game -> move_number++;

  return cell_type;
}

void fill_set(unsigned* set, unsigned size, unsigned range)
{
	set[0] = rand() % range;
  
  for(unsigned i = 1; i < size; i++)
	{
    range--;
    set[i] = rand() % range;
    
    unsigned toAdd, temp;
    unsigned addCount = 0;
		
    do {
			toAdd = addCount;
			temp = set[i] + toAdd;
      addCount = 0;

      for(unsigned j = 0; j < i; j++) {
        if(set[j] <= temp)
          addCount++;
      }
    } while (toAdd != addCount);
      
    set[i] += addCount;
  }
}


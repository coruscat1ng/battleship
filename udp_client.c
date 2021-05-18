#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "game.h"
#include "game.visuals.h"
  
#define PORT 8081
#define MAXLINE 1024
  
const enum colors client_cell_colors[cell_types_number] = {
	[empty] = blue_background,
	[hit] = red_background,
	[unknown] = reset,
	[ship] = green_background
};


int main() {
  
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		fprintf(stderr, "socket creation failed");
		return 0;
	}

	struct sockaddr_in client_addr, server_addr = {
		.sin_family = AF_INET, // IPv4
		.sin_addr.s_addr = inet_addr("127.0.0.1"),
		.sin_port = htons(PORT),
	};

	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	int bytes_number;
 	unsigned size = sizeof(client_addr); 
	
	char buffer[MAXLINE];
  char id;
  
  for(int i = 0;; i++) {
    printf("enter your nickname (max 6 letters)\n");
    while(!scanf("%6s", buffer + 1));
    buffer[0] = 'r';
    usleep(10000);
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    printf("nickname sent\n");
    bytes_number = recvfrom(sockfd, buffer, 2, MSG_WAITALL, (struct sockaddr *) &server_addr, &size);
    if(buffer[0] == 'r') {
      id = buffer[1];
      break;
    } else {
      printf("nickname is already taken\n");
      usleep(100000);
    }
  }
  
 	struct field field = {
		.ships = 0
	};

  printf("registered, your id is %d\n", (int) id);
  printf("enter your field height\n");
  scanf("%d", &field.height);
  printf("enter your field width\n");
  scanf("%d", &field.width);
  printf("enter ships_number\n");
  scanf("%d", &field.remained_ships);
  
  sprintf(buffer, "h%c%dw%ds%d", id, field.height, field.width, field.remained_ships);
  sendto(sockfd, buffer, strlen(buffer) + 1, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
  
  recvfrom(sockfd, buffer, 2, MSG_WAITALL, (struct sockaddr *) &server_addr, &size);
  
  while(buffer[0] == 'q') {
    printf("\033[%dD", printf("#%3d in queue", (int) buffer[1]));
    fflush(stdout);
    buffer[0] = 'h';
    buffer[1] = id;
    sendto(sockfd, buffer, strlen(buffer) + 1, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    recvfrom(sockfd, buffer, 2, MSG_WAITALL, (struct sockaddr *) &server_addr, &size);
  }

	struct game game = {
		.field = &field,
		.move_number = 0,
		.name = "",
		.score = 0,
		.streak = 0
	};
  
  printf("\ngame started\n");
  buffer[0] = 'c';
  sendto(sockfd, buffer, 2, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
  
  int cells_number = field.width * field.height;
  unsigned moves[cells_number];
	
  srand(time(0));
  fill_set(moves, cells_number, cells_number);
  
 	start(&game);
 
  
	#define DELAY 600000

	while(game.move_number < cells_number) {
		for(int i = 0; i < 500; i++) {
			bytes_number = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &server_addr, &size);
		
			if(bytes_number > 0) {
				buffer[bytes_number] = '\0';
        switch (buffer[0]) {
          case 'm':
          {
            int line = moves[game.move_number] / field.width;
            int column = 1 + moves[game.move_number] - field.width * line;
            line++;
            color_cell(column, line, client_cell_colors[empty]);
            game.move_number++;

            fflush(stdout);
            
            break;
          }
          case 'h':
          {
            int line = moves[game.move_number] / field.width;
            int column = 1 + moves[game.move_number] - field.width * line;
            line++;
            
            color_cell(column, line, client_cell_colors[hit]);
            field.remained_ships--;
            game.move_number++;
            fflush(stdout);
            
            break;
          }
          case 's':
          {
            int score;
            sscanf(buffer, "s%d", &score);
						game.score = score;
            end(&game);            
						close(sockfd);
            return 0;
          }
          default:
            break;
        }
        sprintf(buffer, "m%c%u", id, moves[game.move_number]);
        sendto(sockfd, buffer, strlen(buffer) + 1, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        break;
			}
		}
		usleep(DELAY);
    //sendto(sockfd, "d", 2, 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
  }
	close(sockfd);
	return 0;
}

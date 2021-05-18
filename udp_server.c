#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "game.h"
#include "game.visuals.h"
#include "udp_server.h"

#define PORT 8081
#define MAXLINE 1024
#define MAX_CLIENTS 8
#define MAX_REQUESTS 1000
#define DELAY 600000

int main() {
  
 	int sockfd = create_socket();
	if(!sockfd) return 0;
  
  int clients_number = 0, fields_number = 0, 
			active_id = 0, bytes_number;

	char buffer[MAX_CLIENTS];
  
  unsigned int move, requests = 0;
	 
	struct sockaddr_in clients_addr[MAX_CLIENTS], client_addr; 
 	unsigned size = sizeof(client_addr);
  
	struct game games[MAX_CLIENTS];
  struct field fields[MAX_CLIENTS];
  
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
	
  srand((unsigned) time(0));
  while(requests < MAX_REQUESTS) {
    
    for(int i = 0; i < 500; i++) {
      bytes_number = recvfrom(sockfd, (char *)buffer, MAXLINE,  MSG_WAITALL, (struct sockaddr *) &client_addr,  &size);
      
			if(bytes_number > 0) {       
				buffer[bytes_number] = '\0';
        switch (buffer[0]) {
          case 'h':
          {
            int id = buffer[1] - 60;
            if(fields_number == id) {
              fields_number++;

             	int jump = 0;
        			for(;active_id < id; active_id++) jump += (games[active_id].field -> height << 1) + 3;

              if(jump > 0) printf("\033[%dB", jump);
     
              sscanf(buffer + 2, "%dw%ds%d", &fields[id].height, &fields[id].width, &fields[id].remained_ships);
							
							fields[id].ships = malloc(sizeof(unsigned) * fields[id].remained_ships);
              fill_set(fields[id].ships, fields[id].remained_ships, fields[id].height * fields[id].width);
							games[id].move_number = 0;
              games[id].field = &fields[id];

              start(&games[id]);

              buffer[0] = 'd';
            
						} else {
              sprintf(buffer, "q%c", id - fields_number);
            }
            break;
          }
					case 'm':
					{
            int jump = 0, id = buffer[1] - 60;
            
            if(active_id > id) {
              while(active_id > id) {
                active_id--;
                jump += (games[active_id].field -> height << 1) + 3;
              }
              printf("\033[%dA", jump);
            }
            
            if(active_id < id) {
              while(active_id < id) {
                jump += (games[active_id].field -> height << 1) + 3;
                active_id++;
              }
              printf("\033[%dB", jump);
            }
            
						move = atoi(buffer + 2);
            switch (process_move_visualized(&games[id], move)) {
							case empty:
								buffer[0] = 'm';
								break;
            	case hit:
              	if(!games[id].field -> remained_ships) requests = MAX_REQUESTS;
              	buffer[0] = 'h';
								break;
							default: 
								buffer[0] = 'd';
								break;
            }

            fflush(stdout);
            break;
					}
					case 'r':
            if(clients_number >= MAX_CLIENTS) {
              buffer[0] = 'e';
            } else {
              for(int i = 0; i < clients_number; i++) {
                
                if(!strcmp(games[i].name, buffer + 1)) {
                  buffer[0] = 'u';
                  break;
                }
              }
              if(buffer[0] == 'r') {
                memcpy(&clients_addr[clients_number], &client_addr, sizeof(client_addr));
                strcpy(games[clients_number].name, buffer + 1);
                buffer[1] = clients_number + 60;
								games[clients_number].score = 0;
								games[clients_number].streak = 0;
                games[clients_number].field = 0;
                clients_number++;
              }
            }
            break;
          case 'c':
            buffer[0] = 'd';
            break;
          case 'b':
            return 0;
          default:
            buffer[0] = 'd';
            break;
        }
        sendto(sockfd, buffer, strlen(buffer) + 1, 0, (const struct sockaddr *) &client_addr, size);
        requests++;
      }
    }
    usleep(DELAY);
  }

  int jump = 0;
  while(active_id > 0) {
    active_id--;
    jump += (games[active_id].field -> height << 1) + 3;
  }
  if(jump > 0) {
    printf("\033[%dA", jump);
  }
  for(int i = 0; i < clients_number; i++) {
    sprintf(buffer, "s%d", games[i].score);
    sendto(sockfd, buffer, strlen(buffer) + 1, 0, (const struct sockaddr *) &clients_addr[i], size);
    end(&games[i]);
		printf(move_cursor_down("2"));
		fflush(stdout);
  }

	return 0;
}

int create_socket(void)
{
	int sockfd;
	
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		fprintf(stderr, "socket creation failed");
		return 0;
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET, // IPv4
		.sin_addr.s_addr = inet_addr("127.0.0.1"),
		.sin_port = htons(PORT),
	};

	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	if(bind(sockfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ) {
		fprintf(stderr, "socket creation failed");
		return 0;
	}
	
	return sockfd;
}

void print_addr(const struct sockaddr_in *addr, const char *text)
{
  switch (addr -> sin_family) {
    case AF_INET:
      printf("type: IPv4\n");
      break;
    default:
      printf("type: unknown");
      break;
  }
	printf("port: %d\n", ntohs(addr -> sin_port));
	printf("ip: %s\n", inet_ntoa(addr -> sin_addr));
}

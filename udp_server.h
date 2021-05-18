#ifndef udp_server_h
#define udp server_h
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <fcntl.h>

struct client {
	int id;
	struct sockaddr_in addr;
};

struct clients {
	unsigned clients_number;
	struct client* clients;
};

void process_requests(int socket, struct clients *);
int create_socket(void);

void print_addr(const struct sockaddr_in*, const char *text);


#endif

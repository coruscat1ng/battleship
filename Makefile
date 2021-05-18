SERVER = udp_server game game.visuals
CLIENT = udp_client game game.visuals
GAME.TEST = game game.visuals game.test
CC = clang
FLAGs = -Wall -std=c18
.PHONY = clean compile.game.test

compile: udp_client udp_server

game.test: ${GAME.TEST:%=%.o}
	${CC} ${FLAGs} ${GAME.TEST:%=%.o} -o $@
udp_client: ${CLIENT:%=%.o}
	${CC} ${FLAGs} ${CLIENT:%=%.o} -o $@
udp_server: ${SERVER:%=%.o}
	${CC} ${FLAGs} ${SERVER:%=%.o} -o $@
%.o: %.c
	${CC} ${FLAGs} -c $< -o $@
clean:
	-rm -f *.o udp_server

compile.game.test: game.test

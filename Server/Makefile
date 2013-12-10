FLAGS  = -Wall -g
CC     = gcc
PROG   = server
OBJS   = server.o

all:	${PROG}

clean:
	rm ${OBJS} ${EXE} *~
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

server.o: server.c

server: server.o

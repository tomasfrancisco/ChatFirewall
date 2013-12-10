FLAGS  = -Wall -g
CC     = gcc
PROG   = client
OBJS   = client.o

all:	${PROG}

clean:
	rm ${OBJS} ${EXE} *~
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

client.o: client.c

client: client.o

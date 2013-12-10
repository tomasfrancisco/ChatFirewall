FLAGS  = -Wall -g
CC     = gcc
PROG   = firewall
OBJS   = firewall.o

all:	${PROG}

clean:
	rm ${OBJS} ${EXE} *~
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@

.c.o:
	${CC} ${FLAGS} $< -c

##########################

firewall.o: firewall.c

firewall: firewall.o

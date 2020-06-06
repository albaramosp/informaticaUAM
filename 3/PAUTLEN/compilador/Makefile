# Makefile para automatizar la creacion de los ficheros necesarios para el compilador
# Alba Ramos Pedroviejo
# Andrea Salcedo Lopez

CC = gcc -ansi -pedantic
CFLAGS = -Wall -g
OBJECTS = lex.yy.o y.tab.o compilador.o tablahash.o generacion.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o alfa $(OBJECTS)

compilador.o: compilador.c y.tab.h
	$(CC) $(CFLAGS) -c compilador.c

generacion.o: generacion.c generacion.h
	$(CC) $(CFLAGS) -c generacion.c

tablahash.o: tablahash.c tablahash.h
	$(CC) $(CFLAGS) -c tablahash.c

lex.yy.o: lex.yy.c y.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c y.tab.c

lex.yy.c: alfa.l
	$ flex alfa.l

y.tab.h: alfa.y
	$ bison -d -y -v alfa.y

clean:
	rm alfa *.o lex.yy.c y.tab.c y.tab.h y.output

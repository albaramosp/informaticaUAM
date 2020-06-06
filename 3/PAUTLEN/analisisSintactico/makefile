# Makefile para automatizar la creacion de los ficheros necesarios para el analizador sintactico
# Alba Ramos Pedroviejo
# Andrea Salcedo Lopez

CC = gcc
CFLAGS = -g -Wall
OBJECTS = lex.yy.o y.tab.o pruebaSintactico.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o pruebaSintactico $(OBJECTS)

lex.yy.c: alfa.l
	flex alfa.l

lex.yy.o: lex.yy.c y.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c y.tab.c

y.tab.h: alfa.y
	bison -d -y -v alfa.y

pruebaSintactico.o: pruebaSintactico.c y.tab.h
	$(CC) $(CFLAGS) -c pruebaSintactico.c

clean:
	rm -rf pruebaSintactico  *.o lex.yy.c y.tab.c y.tab.h y.output

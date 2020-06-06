# Makefile para automatizar la creacion de los ficheros necesarios para probar la tabla de simbolos
# Alba Ramos Pedroviejo
# Andrea Salcedo Lopez

CC = gcc
CFLAGS = -g -Wall --debug
OBJECTS = tablaSimbolos.o hash.o prueba_tabla.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o prueba_tabla $(OBJECTS)

hash.o: hash.c
	$(CC) $(CFLAGS) -c hash.c

tablaSimbolos.o: tablaSimbolos.c
	$(CC) $(CFLAGS) -c tablaSimbolos.c

prueba_tabla.o: prueba_tabla.c
	$(CC) $(CFLAGS) -c prueba_tabla.c

clean:
	rm -rf prueba_tabla  *.o


#	Automatiza la creación de los distintos programas de cifrado
#	Ejecutar make para crear los programas
#   CRIPTO
# 	Escuela Politecnica Superior - UAM
# 	AUTHORS: grupo 01, Alba Ramos Pedroviejo y Ana Mª Cidoncha Suárez

LIBS = 
LIB_DIR =  

CC = gcc
CFLAGS = -g -Wall -D_GNU_SOURCE
EXE = afin afin_ext vigenere kasiski flujo ic obtenerClave permutacion
SRCPATH = ./src/
SRCLIBPATH = ./srclib/
INCPATH = ./includes/
OBJPATH = ./obj/

.PHONY: clean all

all: clean folders euclides_ext.o euclides.o afin.o afin_ext.o vigenere.o kasiski.o flujo.o ic.o obtenerClave.o permutacion.o  $(EXE)

folders:
	mkdir $(OBJPATH)

euclides_ext.o: $(SRCPATH)euclides_ext.c
	$(CC) $(CFLAGS) -c $(SRCPATH)euclides_ext.c -o $(OBJPATH)euclides_ext.o

euclides.o: $(SRCPATH)euclides.c
	$(CC) $(CFLAGS) -c $(SRCPATH)euclides.c -o $(OBJPATH)euclides.o

ic.o: $(SRCPATH)ic.c
	$(CC) $(CFLAGS) -c $(SRCPATH)ic.c -o $(OBJPATH)ic.o

obtenerClave.o: $(SRCPATH)obtenerClave.c
	$(CC) $(CFLAGS) -c $(SRCPATH)obtenerClave.c -o $(OBJPATH)obtenerClave.o

afin.o: $(SRCPATH)afin.c
	$(CC) $(CFLAGS) -c $(SRCPATH)afin.c -o $(OBJPATH)afin.o

afin_ext.o: $(SRCPATH)afin_ext.c
	$(CC) $(CFLAGS) -c $(SRCPATH)afin_ext.c -o $(OBJPATH)afin_ext.o

vigenere.o: $(SRCPATH)vigenere.c
	$(CC) $(CFLAGS) -c $(SRCPATH)vigenere.c -o $(OBJPATH)vigenere.o

permutacion.o: $(SRCPATH)permutacion.c
	$(CC) $(CFLAGS) -c $(SRCPATH)permutacion.c -o $(OBJPATH)permutacion.o

kasiski.o: $(SRCPATH)kasiski.c
	$(CC) $(CFLAGS) -c $(SRCPATH)kasiski.c -o $(OBJPATH)kasiski.o

flujo.o: $(SRCPATH)flujo.c
	$(CC) $(CFLAGS) -c $(SRCPATH)flujo.c -o $(OBJPATH)flujo.o

afin: $(OBJPATH)afin.o $(OBJPATH)euclides.o
	$(CC) $(CFLAGS) -o afin $(OBJPATH)afin.o $(OBJPATH)euclides.o -lgmp -L.

afin_ext: $(OBJPATH)afin_ext.o $(OBJPATH)euclides.o
	$(CC) $(CFLAGS) -o afin_ext $(OBJPATH)afin_ext.o $(OBJPATH)euclides.o -lgmp -L.

vigenere: $(OBJPATH)vigenere.o
	$(CC) $(CFLAGS) $(OBJPATH)vigenere.o -o vigenere

permutacion: $(OBJPATH)permutacion.o
	$(CC) $(CFLAGS) $(OBJPATH)permutacion.o -o permutacion

ic: $(OBJPATH)ic.o
	$(CC) $(CFLAGS) -o ic $(OBJPATH)ic.o $(OBJPATH)euclides.o -lgmp -L.

obtenerClave: $(OBJPATH)obtenerClave.o
	$(CC) $(CFLAGS) $(OBJPATH)obtenerClave.o -o obtenerClave -lm

flujo: $(OBJPATH)flujo.o
	$(CC) $(CFLAGS) $(OBJPATH)flujo.o -o flujo

kasiski: $(OBJPATH)kasiski.o
	$(CC) $(CFLAGS) -o kasiski $(OBJPATH)kasiski.o $(OBJPATH)euclides.o -lgmp -L.

euclides_ext: $(OBJPATH)euclides_ext.o
	$(CC) $(CFLAGS) $(OBJPATH)euclides_ext.o -o euclides_ext -lgmp -L.

euclides: $(OBJPATH)euclides.o
	$(CC) $(CFLAGS) -o euclides $(OBJPATH)euclides.o -lgmp -L.

valgrind:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXE)

clean:
	rm -rf $(OBJPATH)
	rm -rf $(EXE)

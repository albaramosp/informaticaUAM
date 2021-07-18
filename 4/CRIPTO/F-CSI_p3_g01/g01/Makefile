
#	Automatiza la creación de los distintos programas
#	Ejecutar make para crear los programas
#   CRIPTO
# 	Escuela Politecnica Superior - UAM
# 	AUTHORS: grupo 01, Alba Ramos Pedroviejo y Ana Mª Cidoncha Suárez

LIBS = 
LIB_DIR =  

CC = gcc
CFLAGS = -g -Wall -D_GNU_SOURCE
EXE = potencia
SRCPATH = ./src/
SRCLIBPATH = ./srclib/
INCPATH = ./includes/
OBJPATH = ./obj/

.PHONY: clean all

all: clean folders potencia.o $(EXE)

folders:
	mkdir $(OBJPATH)

potencia.o: $(SRCPATH)potencia.c
	$(CC) $(CFLAGS) -c $(SRCPATH)potencia.c -o $(OBJPATH)potencia.o

potencia: $(OBJPATH)potencia.o
	$(CC) $(CFLAGS) $(OBJPATH)potencia.o -o potencia -lgmp -L.

valgrind:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXE)

clean:
	rm -rf $(OBJPATH)
	rm -rf $(EXE)

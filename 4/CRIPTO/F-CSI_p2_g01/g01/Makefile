
#	Automatiza la creación de los distintos programas de cifrado
#	Ejecutar make para crear los programas
#   CRIPTO
# 	Escuela Politecnica Superior - UAM
# 	AUTHORS: grupo 01, Alba Ramos Pedroviejo y Ana Mª Cidoncha Suárez

LIBS = 
LIB_DIR =  

CC = gcc
CFLAGS = -g -Wall -D_GNU_SOURCE
EXE = seg-perf desCBC TDEA-CBC avalanche nonLinear SBOX_AES nonLinear_AES
SRCPATH = ./src/
SRCLIBPATH = ./srclib/
INCPATH = ./includes/
OBJPATH = ./obj/

.PHONY: clean all

all: clean folders funcionesDES.o seg-perf.o desCBC.o TDEA-CBC.o avalanche.o nonLinear.o SBOX_AES.o nonLinear_AES.o $(EXE)

folders:
	mkdir $(OBJPATH)

seg-perf.o: $(SRCPATH)seg-perf.c
	$(CC) $(CFLAGS) -c $(SRCPATH)seg-perf.c -o $(OBJPATH)seg-perf.o

desCBC.o: $(SRCPATH)desCBC.c
	$(CC) $(CFLAGS) -c $(SRCPATH)desCBC.c -o $(OBJPATH)desCBC.o

funcionesDES.o: $(SRCPATH)funcionesDES.c
	$(CC) $(CFLAGS) -c $(SRCPATH)funcionesDES.c -o $(OBJPATH)funcionesDES.o

TDEA-CBC.o: $(SRCPATH)TDEA-CBC.c
	$(CC) $(CFLAGS) -c $(SRCPATH)TDEA-CBC.c -o $(OBJPATH)TDEA-CBC.o

nonLinear.o: $(SRCPATH)nonLinear.c
	$(CC) $(CFLAGS) -c $(SRCPATH)nonLinear.c -o $(OBJPATH)nonLinear.o

avalanche.o: $(SRCPATH)avalanche.c
	$(CC) $(CFLAGS) -c $(SRCPATH)avalanche.c -o $(OBJPATH)avalanche.o

SBOX_AES.o: $(SRCPATH)SBOX_AES.c
	$(CC) $(CFLAGS) -c $(SRCPATH)SBOX_AES.c -o $(OBJPATH)SBOX_AES.o

nonLinear_AES.o: $(SRCPATH)nonLinear_AES.c
	$(CC) $(CFLAGS) -c $(SRCPATH)nonLinear_AES.c -o $(OBJPATH)nonLinear_AES.o

seg-perf: $(OBJPATH)seg-perf.o
	$(CC) $(CFLAGS) $(OBJPATH)seg-perf.o -o seg-perf -lm

avalanche: $(OBJPATH)avalanche.o $(OBJPATH)funcionesDES.o
	$(CC) $(CFLAGS) $(OBJPATH)avalanche.o $(OBJPATH)funcionesDES.o -o avalanche -lm

TDEA-CBC: $(OBJPATH)TDEA-CBC.o $(OBJPATH)funcionesDES.o
	$(CC) $(CFLAGS) -o TDEA-CBC $(OBJPATH)TDEA-CBC.o $(OBJPATH)funcionesDES.o

nonLinear: $(OBJPATH)nonLinear.o $(OBJPATH)funcionesDES.o
	$(CC) $(CFLAGS) -o nonLinear $(OBJPATH)nonLinear.o $(OBJPATH)funcionesDES.o

nonLinear_AES: $(OBJPATH)nonLinear_AES.o 
	$(CC) $(CFLAGS) -o nonLinear_AES $(OBJPATH)nonLinear_AES.o 

desCBC: $(OBJPATH)desCBC.o $(OBJPATH)funcionesDES.o
	$(CC) $(CFLAGS) -o desCBC $(OBJPATH)desCBC.o $(OBJPATH)funcionesDES.o 

SBOX_AES: $(OBJPATH)SBOX_AES.o
	$(CC) $(CFLAGS) $(OBJPATH)SBOX_AES.o -o SBOX_AES -lm


valgrind:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXE)

clean:
	rm -rf $(OBJPATH)
	rm -rf $(EXE)

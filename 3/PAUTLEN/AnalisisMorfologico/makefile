# Makefile para automatizar la creacion de los ficheros necesarios para el analizador morfologico
# Alba Ramos Pedroviejo
# Andrea Salcedo Lopez

all: lex.yy.c pruebaMorfo

lex.yy.c: alfa.l
		flex alfa.l

pruebaMorfo.o: pruebaMorfo.c
		gcc -c pruebaMorfo.c -o pruebaMorfo.o

pruebaMorfo: pruebaMorfo.o
		gcc -Wall -o pruebaMorfo lex.yy.c pruebaMorfo.o -lfl

clean:
		rm -rf pruebaMorfo.o
		rm -rf pruebaMorfo
		rm -rf lex.yy.c

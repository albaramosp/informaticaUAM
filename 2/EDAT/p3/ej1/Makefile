CC = gcc -g
CFLAGS = -Wall -ansi -pedantic
EXE = table_test type_test

all: $(EXE)

table_test: table_test.o table.o
	$(CC) $(CFLAGS) -o table_test table_test.o table.o

type_test: type_test.o type.o
	$(CC) $(CFLAGS) -o type_test type_test.o type.o

table_test.o: table_test.c
	$(CC) $(CFLAGS) -c table_test.c

type_test.o:type_test.c
	$(CC) $(CFLAGS) -c type_test.c

type.o: type.c type.h
	$(CC) $(CFLAGS) -c type.c

table.o: table.c table.h type.h
	$(CC) $(CFLAGS) -c table.c

gdb:
	gdb -tui ./table_test


clean :
	rm -rf *.o $(EXE)



##CC=gcc
##CFLAGS=-I.
##DEPS = hellomake.h
##OBJ = hellomake.o hellofunc.o

##%.o: %.c $(DEPS)
##	$(CC) -c -o $@ $< $(CFLAGS)

##hellomake: $(OBJ)
##	$(CC) -o $@ $^ $(CFLAGS)

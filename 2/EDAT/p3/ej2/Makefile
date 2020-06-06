CC = gcc -g
CFLAGS = -Wall -ansi -pedantic
EXE = table_test type_test score suggest

all: $(EXE)

table_test: table_test.o table.o
	$(CC) $(CFLAGS) -o table_test table_test.o table.o

type_test: type_test.o type.o
	$(CC) $(CFLAGS) -o type_test type_test.o type.o

score: score.o table.o odbc.o type.o
	$(CC) $(CFLAGS) -o score score.o table.o odbc.o type.o -lodbc

suggest: suggest.o table.o odbc.o type.o
	$(CC) $(CFLAGS) -o suggest suggest.o table.o odbc.o type.o -lodbc

table_test.o: table_test.c
	$(CC) $(CFLAGS) -c table_test.c

type_test.o:type_test.c
	$(CC) $(CFLAGS) -c type_test.c

score.o: score.c table.h odbc.h type.h
	$(CC) $(CFLAGS) -c -I. score.c

suggest.o: suggest.c table.h odbc.h type.h
	$(CC) $(CFLAGS) -c -I. suggest.c

type.o: type.c type.h
	$(CC) $(CFLAGS) -c type.c

table.o: table.c table.h type.h
	$(CC) $(CFLAGS) -c table.c

odbc.o: odbc.c odbc.h
	$(CC) $(CFLAGS) -c odbc.c

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

#	Automates the creation of the server
#	Execute make to build all targets and compile the library
#	Execute make valgrind to test memory leaks - you won't find any:) -
#   2361 - Redes II
# 	Escuela Politecnica Superior - UAM
# 	AUTHORS: pair 15, Alba Ramos Pedroviejo and Nicolas Serrano Salas

CC = gcc
CFLAGS = -g -Wall -ansi -pedantic -pthread
EXE = server
STATICLIB = static_lib
SRCPATH = ./src/
SRCLIBPATH = ./srclib/
INCPATH = ./includes/
OBJPATH = ./obj/
LIBPATH = ./lib/

all: clean folders picohttpparser.o sockets.o request_response.o static_lib  $(EXE)

folders:
	mkdir $(OBJPATH) $(LIBPATH)

sockets.o: $(SRCLIBPATH)sockets.c
	$(CC) $(CFLAGS) -c $(SRCLIBPATH)sockets.c -o $(OBJPATH)sockets.o

request_response.o: $(SRCLIBPATH)request_response.c
	$(CC) $(CFLAGS) -c $(SRCLIBPATH)request_response.c -o $(OBJPATH)request_response.o

picohttpparser.o: $(SRCLIBPATH)picohttpparser.c
	$(CC) $(CFLAGS) -c $(SRCLIBPATH)picohttpparser.c -o $(OBJPATH)picohttpparser.o

static_lib:
	ar rcs $(LIBPATH)lib$(STATICLIB).a $(OBJPATH)sockets.o $(OBJPATH)request_response.o $(OBJPATH)picohttpparser.o
	ranlib $(LIBPATH)lib$(STATICLIB).a

server.o: $(SRCPATH)server.c
	$(CC) $(CFLAGS) -c $(SRCPATH)server.c -o $(OBJPATH)server.o

server: server.o
	$(CC) $(CFLAGS) $(OBJPATH)server.o -L$(LIBPATH) -l$(STATICLIB) -o server

valgrind:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXE)

clean:
		rm -rf $(OBJPATH)
		rm -rf $(LIBPATH)
		rm -rf $(EXE)

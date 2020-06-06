TARGET = ../target

CC = gcc
CFLAGS = -g -Wall -pthread -I.
LDLIBS = -lrt -lncurses

BOLD=\e[1m
NC=\e[0m

all: jefe nave simulador monitor

clean:
	rm -rf /dev/shm/shm_estado
	rm -rf /dev/shm/shm_tuberias
	rm -rf /dev/shm/sem.sem_mutex_mapa
	rm -rf /dev/shm/sem.sem_mutex_contC
	rm -rf /dev/shm/sem.sem_monitor
	rm -rf /dev/shm/sem.sem_jefes
	rm -r -f $(TARGET)

simulador:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c simulador.c -o $(TARGET)/simulador -lrt -lm

jefe:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c jefe.c -o $(TARGET)/jefe -lrt -lm

nave:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) mapa.c nave.c -o $(TARGET)/nave -lrt -lm

monitor:
	mkdir -p $(TARGET)
	$(CC) $(CFLAGS) gamescreen.c mapa.c monitor.c -o $(TARGET)/monitor -lrt -lncurses -lm

/**
 * @brief Programa que muestra el estado de la simulacion en la terminal de forma mas visual. Debe ser ejecutado
 * desde un terminal diferente al del proceso simulador, y finalizado a mano con Ctrl+C.
 * @file monitor.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 11-04-2019
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <simulador.h>
#include <gamescreen.h>
#include <mapa.h>

#define SEM_MONITOR "/sem_monitor"
#define MUTEX_MAPA "/sem_mutex_mapa"
#define SHM_MAPA "/shm_estado"

/**
 * @brief: Funcion que imprime por pantalla todos los simbolos de las casillas del mapa recibido como parametro.
 * @param: puntero a tipo_mapa
 * @return: void
 */
void mapa_print(tipo_mapa *mapa){
	int i,j;

	for(j=0;j<MAPA_MAXY;j++) {
		for(i=0;i<MAPA_MAXX;i++) {
			tipo_casilla cas = mapa_get_casilla(mapa,j, i);
			screen_addch(j, i, cas.simbolo);
		}
	}
	screen_refresh();
}

/*Proceso monitor, ejecutar desde otro terminal*/
int main() {
	sem_t *sem_monitor = NULL; /*Para esperar a que el simulador tenga todo creado antes de empezar*/
	sem_t *mutexMapa = NULL; /*Para leer del mapa de uno en uno*/
	int memoriaEstado; /*Memoria compartida para guardar estado del mapa y de la simulacion*/
	tipo_mapa *mapa;

	/*Si el monitor empieza antes, crea el semaforo, inicializado a 0, por lo que esperara igualmente al simulador*/
	sem_monitor = sem_open(SEM_MONITOR, O_CREAT, S_IRUSR | S_IWUSR, 0);
	if (sem_monitor == SEM_FAILED) {
		perror("Error al crear semaforo de aviso al monitor en el monitor");
		return EXIT_FAILURE;
	}

	sem_wait(sem_monitor); /*No empieza hasta que el simulador tenga todos los recursos creados*/

	memoriaEstado = shm_open(SHM_MAPA, O_RDWR,  0);
	if(memoriaEstado == -1) {
		perror("Error al crear memoria compartida mapa en monitor");
		return EXIT_FAILURE;
	}

	mapa =(tipo_mapa*) mmap(NULL, sizeof(tipo_mapa), PROT_READ | PROT_WRITE, MAP_SHARED, memoriaEstado, 0);
	if(mapa == MAP_FAILED) {
		perror("Error al hacer el mapeo del mapa en monitor");
		shm_unlink(SHM_MAPA);
		return EXIT_FAILURE;
	}

	if ((mutexMapa = sem_open(MUTEX_MAPA, 0)) == SEM_FAILED){
		perror("Error al abrir semaforo de mutex mapa en el monitor");
		return EXIT_FAILURE;
	}

	screen_init();

	while(1){
		mapa_print(mapa);

		usleep(SCREEN_REFRESH); /*Espera entre impresiones*/
	}


	screen_end();

	sem_close(sem_monitor);
	sem_unlink(SEM_MONITOR);

	sem_close(mutexMapa);

	close(memoriaEstado);
	munmap(mapa, sizeof(tipo_mapa));

	exit(EXIT_SUCCESS);
}

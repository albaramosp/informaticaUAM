/**
 * @brief Programa que simula el comportamiento del consumidor, que debe esperar a que haya algun hueco lleno en la cola para consumir y debe indicar
 * con cada elemento consumido que se ha vaciado un hueco. Para esto se utilizaran semaforos, ademas de para controlar el acceso a la seccion
 * critica que implica la extraccion e insercion de productos en la cola.
 * @file ejercicio3_consumidor.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 04-04-2019
 */

#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "queue.h"

#define SHM_NAME "/shm_productos"
#define SEM_RES "/sem_recurso"
#define SEM_EMPTY "/sem_vacio"
#define SEM_FULL "/sem_lleno"
#define MAXQUEUE 10


int main(void) {
	sem_t *sem_recurso = NULL;
	sem_t *sem_lleno = NULL;
	sem_t *sem_vacio = NULL;
	int fd_shm;
	char c;
	Queue *cola;

	fd_shm = shm_open(SHM_NAME, O_RDWR,  0);

	if(fd_shm == -1) {
		perror("Error al abrir memoria consumidor");
		return EXIT_FAILURE;
	}

	cola = (Queue *)mmap(NULL, sizeof(cola), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	if(cola == MAP_FAILED) {
		perror("Error al hacer el mapeo");
		return EXIT_FAILURE;
	}

	if ((sem_recurso = sem_open(SEM_RES, 0)) == SEM_FAILED) {
		perror("Error al crear semaforo");
		munmap(cola, sizeof(cola));
		return EXIT_FAILURE;
	}

  if ((sem_lleno = sem_open(SEM_FULL, 0)) == SEM_FAILED) {
    perror("Error al abrir semaforo lleno");
    munmap(cola, sizeof(cola));
    return EXIT_FAILURE;
  }

  if ((sem_vacio = sem_open(SEM_EMPTY, 0)) == SEM_FAILED) {
    perror("Error al abrir semaforo vacio");
    munmap(cola, sizeof(cola));
    return EXIT_FAILURE;
  }

	while (1) {
    sem_wait(sem_lleno); /*No consume a no ser que haya al menos un hueco lleno en la cola*/
			sem_wait(sem_recurso); /*mutex*/
			  c = queue_extract(cola);
			  /*Cuando consuma '\0', termina*/
			  if(c == '\0'){
				  break;
			  }

			  printf("Consumido: %c\n", c);
			sem_post(sem_recurso);
		sem_post(sem_vacio); /*Indica que se ha vaciado un hueco de la cola*/
    }

	munmap(cola, sizeof(cola));
	return EXIT_SUCCESS;
}

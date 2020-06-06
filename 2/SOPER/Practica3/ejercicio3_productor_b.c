/**
 * @brief Programa que simula el comportamiento del productor, que debe esperar a que haya huecos vacios en la cola para producir y debe indicar
 * con cada elemento producido que se ha llenado un hueco. Para esto se utilizaran semaforos, ademas de para controlar el acceso a la seccion
 * critica que implica la extraccion e insercion de productos en la cola.
 * @file ejercicio3_productor.c
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
 #include <semaphore.h>
 #include "queue.h"

#define SHM_NAME "shm_productos" /*Nombre de fichero no puede comenzar por '/'*/
#define SEM_RES "/sem_recurso"
#define SEM_EMPTY "/sem_vacio"
#define SEM_FULL "/sem_lleno"
#define MAXQUEUE 10

int main(void) {
	sem_t *sem_recurso = NULL; /*Controla el acceso a la cola para insertar y extraer productos*/
	sem_t *sem_lleno = NULL; /*Controla los huecos llenos de la cola, para que el consumidor no consuma si no hay nada*/
	sem_t *sem_vacio = NULL; /*Controla los huecos vacios de la cola, para que el productor no produzca de mas*/
	int fd;
	pid_t pid;
	char c;
	Queue *cola;

	printf("Ejecutando productor\n");

	fd = open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

	if(fd == -1) {
		perror("Error al crear memoria productor");
		return EXIT_FAILURE;
	}

	if(ftruncate(fd, sizeof(Queue*)) == -1) {
		perror("Error al hacer truncate");
		unlink(SHM_NAME);
		close(fd);
		return EXIT_FAILURE;
	}


	cola = (Queue *)mmap(NULL, sizeof(cola), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(cola == MAP_FAILED) {
		perror("Error al hacer el mapeo");
		unlink(SHM_NAME);
		close(fd);
		return EXIT_FAILURE;
	}

	if ((sem_recurso = sem_open(SEM_RES, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) { /*El primero que llega puede entrar*/
		perror("Error al crear semaforo recurso");
		munmap(cola, sizeof(cola));
		unlink(SHM_NAME);
		close(fd);
		return EXIT_FAILURE;
	}

	if ((sem_lleno = sem_open(SEM_FULL, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
		perror("Error al crear semaforo lleno");
		sem_unlink(SEM_RES);
		sem_close(sem_recurso);
		munmap(cola, sizeof(cola));
		unlink(SHM_NAME);
		close(fd);
		return EXIT_FAILURE;
	}

	if ((sem_vacio = sem_open(SEM_EMPTY, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, MAXQUEUE-1)) == SEM_FAILED) { /*Cola circular: sacrificas un hueco*/
		perror("Error al crear semaforo vacio");
		sem_unlink(SEM_RES);
		sem_close(sem_recurso);
		sem_unlink(SEM_FULL);
		sem_close(sem_lleno);
		munmap(cola, sizeof(cola));
		unlink(SHM_NAME);
		close(fd);
		return EXIT_FAILURE;
	}

	//Inicializar los campos apuntadores de la cola, usados para insertar y extraer elementos
	queue_setFront(cola, 0);
	queue_setRear(cola, 0);

	pid = fork();
   if (pid < 0) {
	 perror("Error fork");
	 exit(EXIT_FAILURE);
   }

	 /*hijo: ejecutar consumidor*/
  if (pid == 0) {
	 //Importante enviar como primer argumento el nombre del programa, es lo que recibira el main del programa que se ejecute
	 execlp("./ejercicio3_consumidor_b", "ejercicio3_consumidor_b", (char *)NULL);
	 perror("fallo en exec consumidor");
	 exit(EXIT_FAILURE);


  }
	/*Padre: productor*/
	else {
	  printf("\nPuedes meter caracteres hasta que te canses. Introduce ctrl+d para acabar.");
	  printf("\nSi separas los caracteres por enter, veras la ejecucion paso a paso.");
	  printf("\nSi los separas por espacio y despues pulsas enter, veras toda la ejecucion de golpe.");
	  printf("\nEsta ultima opcion es util si introduces muchos caracteres y quieres ver que los semaforos funcionan correctamente.");
	  printf("\nPuedes empezar: \n");
		while (1) {
			scanf(" %c", &c);
			/*Si detecta EOF, introduce '\0' y deja de producir*/
			if(feof(stdin) == 1){
  			 c = '\0';
  			 sem_wait(sem_vacio); /*Si todos los huecos se han rellenado, debe esperar*/
  				sem_wait(sem_recurso); /*mutex*/
  					queue_insert(cola, c);
  					printf("Producido\n");
  				sem_post(sem_recurso);
  			 sem_post(sem_lleno); /*Indica que hay un hueco lleno mas*/

			   break;
			}

			/*Si no es EOF, lo inserta, controlando que haya espacio*/
			sem_wait(sem_vacio);
				sem_wait(sem_recurso);
					queue_insert(cola, c);
					printf("Producido\n");
				sem_post(sem_recurso);
			 sem_post(sem_lleno);
		}

	}

	wait(NULL);

	munmap(cola, sizeof(cola));
	sem_unlink(SEM_RES);
	sem_close(sem_recurso);
	sem_unlink(SEM_FULL);
	sem_close(sem_lleno);
	sem_unlink(SEM_EMPTY);
	sem_close(sem_vacio);
	unlink(SHM_NAME);
	close(fd);
	return EXIT_SUCCESS;
}

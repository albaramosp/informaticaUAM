/**
 * @brief Programa en el que varios procesos hijos escriben en la misma zona de memoria y actualizan variables de ella,
 * y el padre lee las actualizaciones.
 * @file ejercicio2.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define SHM_NAME "/shm_clientes"
#define SEM_NAME "/sem_recurso"
#define NAME_MAX 100

volatile sig_atomic_t salir = 0;

/*Estructura que sera escrita utilizando la zona de memoria compartida*/
typedef struct{
	int previous_id; //!< Id of the previous client.
	int id; //!< Id of the current client.
	char name[NAME_MAX]; //!< Name of the client.
} ClientInfo;

#define MAXBUF 10

void manejador() {
	salir = 1;
	printf("Valor de salir cambiado\n");
}

/**
 * @brief Funcion que recibe por argumento el numero de hijos a crear. Cada hijo actualiza valores de la zona de memoria compartida.
 * El padre lee de esa zona.
 * @param argc entero con el numero de parametros
 * @param argv cadena con los parametros
 */
int main(int argc, char const *argv[]) {
	sem_t *sem_recurso = NULL;
	pid_t pid;
	ClientInfo *example_struct;
	int i;
	int fd_shm;
	int error;

	if (argc != 2){
		printf("\nError de argumentos: introducir SOLAMENTE numero de hijos a crear\n");
		exit(EXIT_FAILURE);
	}

  if ((sem_recurso = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
    perror("sem_open recurso");
    exit(EXIT_FAILURE);
  }

	fd_shm = shm_open(SHM_NAME,
		O_RDWR | O_CREAT | O_EXCL,
		S_IRUSR | S_IWUSR);

	if (fd_shm == -1) {
		fprintf (stderr, "Error al crear la memoria compartida \n");
		sem_unlink(SEM_NAME);
		return EXIT_FAILURE;
	}

	error = ftruncate(fd_shm, sizeof(ClientInfo));
	if (error == -1) {
		fprintf (stderr, "Error al cambiar el tamanyo de la zona de memoria compartida \n");
		shm_unlink(SHM_NAME);
		sem_unlink(SEM_NAME);
		return EXIT_FAILURE;
	}

	example_struct = (ClientInfo *)mmap(NULL, sizeof(*example_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
	if(example_struct == MAP_FAILED) {
		fprintf (stderr, "Error al mapear la zona de memoria compartida\n");
		shm_unlink(SHM_NAME);
		sem_unlink(SEM_NAME);
		return EXIT_FAILURE;
	}

	close(fd_shm);
	shm_unlink(SHM_NAME);
	sem_unlink(SEM_NAME);

	/*Inicializacion de los contenidos*/
	example_struct->id = 0;
	example_struct->previous_id = -1;


	srand(time(NULL));

	/*Creacion de hijos*/
	for (i = 0; i < atoi(argv[1]); i++){
		pid = fork();

		if (pid == -1){
			perror("fork error");
			shm_unlink(SHM_NAME);
			sem_unlink(SEM_NAME);
			exit(EXIT_FAILURE);
		}

		/*hijo*/
		else if (pid == 0){
			sleep(rand() % (10) + 1); //Duerme entre 1 y 10 segundos

			/* Actualizamos los valores de memoria*/
			sem_wait(sem_recurso);
				(example_struct->previous_id)++;

				char nombre[NAME_MAX];
				printf("\nIntroducir el nombre del cliente a dar de alta: ");
				scanf("%s", nombre);
				memcpy(example_struct->name, nombre, sizeof(nombre));

				(example_struct->id)++;
			sem_post(sem_recurso);

			printf("El hijo %d va a mandar senyal\n", i);
			kill(getppid(), SIGUSR1);

			exit(EXIT_SUCCESS);
		}

		/*padre*/
		else {
			struct sigaction act;
			act.sa_handler = manejador;
			sigemptyset(&(act.sa_mask));

			if (sigaction(SIGUSR1, &act, NULL) < 0) {
				perror("ERROR CAPTURA SIGUSR1");
				shm_unlink(SHM_NAME);
				sem_unlink(SEM_NAME);
				exit(EXIT_FAILURE);
			}

			/*Espera a que llegue la senyal SIGUSR1*/
			while(salir == 0)
				pause();
			salir = 0;

			/*Leemos los datos de memoria*/
			sem_wait(sem_recurso);
				printf("Usuario: %s\n", example_struct->name);
				printf("Su id es: %d\n", example_struct->id);
				printf("El id del cliente anterior es: %d\n", example_struct->previous_id);
			sem_post(sem_recurso);
		}
	}

	munmap(example_struct, sizeof(*example_struct));
	sem_close(sem_recurso);

	for (i = 0; i < atoi(argv[1]); i++){
		wait(NULL);
	}

	return EXIT_SUCCESS;
}

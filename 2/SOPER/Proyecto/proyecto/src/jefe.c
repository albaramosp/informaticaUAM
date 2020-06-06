/**
 * @brief Programa que recibe ordenes del simulaodr y las procesa, decidiendo que ordenes enviar a las naves de manera aleatoria.
 * @file jefe.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 11-04-2019
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <mapa.h>
#include <time.h>
#include <semaphore.h>
#include <simulador.h>

#define MQ_NAME "/mq_naveSimulador"
#define SHM_TUB "/shm_tuberias"
#define SHM_NAME "/shm_estado"
#define SEM_JEFES "/sem_jefes"

Tuberia *tuberia; /*!<Tuberia para recibir los mensajes del simulador*/
int memoriaTuberia; /*!<Memoria compartida para almacenar la tuberia*/
sem_t *sem_jefes = NULL; /*!<Semaforo para indicar que el jefe ha esperado a que todas las naves esten creadas*/

/**
 * @brief Funcion para liberar recursos
 * @return void
 */
void liberar (){
	sem_close(sem_jefes);
 	munmap(tuberia, sizeof(Tuberia));
 	close(memoriaTuberia);
}


/*Proceso jefe*/
int main(int argc, char const *argv[]) {
	int equipo = atoi(argv[1]);

	printf("Jefe %d: Arrancado\n", equipo);

	int i, j, cont = 0;
	int pid;
	Orden orden;

	/*Manejador para ignorar SIGINT*/
	struct sigaction act2;
	act2.sa_handler = SIG_IGN;
	sigemptyset(&(act2.sa_mask));
	sigaddset(&(act2.sa_mask), SIGALRM);
	act2.sa_flags = 0;

	if (sigaction(SIGINT, &act2, NULL) < 0) {
		perror("Error al capturar senyal de sigterm en jefe");
		exit(EXIT_FAILURE);
	}

	printf("Jefe %d: gestionando SHM de las tuberias\n", equipo);
	memoriaTuberia = shm_open(SHM_TUB, O_RDWR,  0);
	if(memoriaTuberia == -1) {
		perror("Error al crear memoria compartida tuberia jefe");
		return EXIT_FAILURE;
	}

	tuberia =(Tuberia*) mmap(NULL, sizeof(Tuberia), PROT_READ | PROT_WRITE, MAP_SHARED, memoriaTuberia, 0);
	if(tuberia == MAP_FAILED) {
		perror("Error al hacer el mapeo");
		liberar();
		return EXIT_FAILURE;
	}

	if ((sem_jefes = sem_open(SEM_JEFES, 0)) == SEM_FAILED) {
		perror("Error al abrir semaforo de jefes listos en el jefe");
		liberar();
		return EXIT_FAILURE;
	}

	srand(getpid()*time(NULL)); //Semilla para generar el numero aleatorio

	printf("Jefe %d: creando naves\n", equipo);
		for (i = 0; i < N_NAVES; i++){
			pid = fork();
			if (pid < 0){
				perror("Error al hacer fork en jefe para crear naves");
				liberar();
				return EXIT_FAILURE;
			}

			else if (pid == 0){
				char eq[50];
				char contador[50];
				sprintf(eq, "%d", equipo);//Convertimos el contador a una cadena
				argv[1] = eq;//Lo igualamos a un const char* para pasarselo a la funcion execlp

				sprintf(contador, "%d", cont);//Convertimos el contador a una cadena
				argv[2] = contador;//Lo igualamos a un const char* para pasarselo a la funcion execlp

				if (execlp("./nave", "./nave",	argv[1], argv[2], (char *)NULL) == -1){
					liberar();
					perror("\nError al hacer exec en el jefe para crear naves");
				}
			}
													//(rand() %  (upper - lower + 1)) + lower;
			else{
				cont++;

				if (cont == N_NAVES){
						printf("Jefe %d: SE HAN CREADO TODAS LAS NAVES\n", equipo);
						sem_post(sem_jefes); //Indicar al simulador que los jefes ya tienen todo creado

						while(1){
							int mov = (rand() % 2);//Generamos un numero aleatorio, 0 o 1, y envia una de las dos acciones a todas sus naves
							printf("Jefe %d: crea el numero aleatorio %d para enviar una accion a sus naves\n", equipo, mov);

							printf("\n");
							printf("Jefe %d:  va a leer de la tuberia\n", equipo);
							close(tuberia->fd[equipo][1]);
							read(tuberia->fd[equipo][0], &orden, sizeof(Orden));
							printf("Jefe %d: termina de leer de la tuberia\n", equipo);

							switch(orden.accion){
								case TURNO:
									printf("Jefe %d:  ha recibido  la señal TURNO del simulador\n", equipo);

									if (mov == 0){
										printf("Jefe %d: va a enviar la señal ATACAR a todas sus naves\n", equipo);
										orden.accion = ATACAR; /*Las naves son las que deciden a quien atacar*/

									} else {
										printf("Jefe %d: va a enviar la señal MOVER a todas sus naves\n", equipo);
										orden.accion = MOVER; /*Las naves son las que deciden a donde moverse*/
									}

									//Indexar en el array de tuberias --> i = ( N_EQUIPOS*(equipo + 1) ) + numNave

									for(j = 0; j < N_NAVES; j++){ //j indica el numero de la nave a la que le escribimos
										printf("Jefe %d: va a enviar la orden  a nave %d\n", equipo, j);
										close(tuberia->fd[( N_EQUIPOS*(equipo + 1) ) + j][0]);
										write(tuberia->fd[( N_EQUIPOS*(equipo + 1) ) + j][1], &orden, sizeof(Orden));
										printf("Jefe %d: ya ha enviado la orden a nave %d\n", equipo, j);
									}

									break;

								case DESTRUIR:
									printf("Jefe %d:  ha recibido la señal DESTRUIR del simulador\n", equipo);
									printf("Jefe %d:  tiene que destruir la nave %d\n", equipo, orden.dest);

									close(tuberia->fd[( N_EQUIPOS *(equipo + 1) ) + orden.dest][0]);
									write(tuberia->fd[( N_EQUIPOS *(equipo + 1) ) + orden.dest][1], &orden, sizeof(Orden));

									break;

								case FIN:
									printf("\nJefe %d: ACCION FIN recibida\n", equipo);

									/*Jefe destruye y espera a todas la naves, no deja hijos huerfanos*/
									for (j = 0; j < N_NAVES; j++){
										orden.accion = DESTRUIR;

										printf("\nJefe %d: espera a destruccion de nave %d\n", equipo, j);

										close(tuberia->fd[( N_EQUIPOS*(equipo + 1) ) + j][0]);
										write(tuberia->fd[( N_EQUIPOS*(equipo + 1) ) + j][1], &orden, sizeof(Orden));
									}

									for (j = 0; j < N_NAVES; j++){
											wait(NULL);
									}

									/*Se liberan los recursos*/
									liberar();

									printf("\nJefe %d: ACABA\n", equipo);
									exit(EXIT_SUCCESS);
									break;

								default:
									break;
							}
						}
				}
		}
	}

	/*Esperar a que acaben las naves*/
	for (i = 0; i < N_NAVES; i++){
		wait(NULL);
	}

    exit(EXIT_SUCCESS);
}

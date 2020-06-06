/**
 * @brief Programa que se encarga de ejecutar el codigo de una nave, el cual recibe una accion enviada por su proceso jefe
 *  para posteriormente, enviárselo al simulador para que ejecute las acciones necesarias una vez recibida la información
 *  por la Cola de Mensajes.
 * @file nave.c
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
#include <semaphore.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <mapa.h>
#include <time.h>
#include <simulador.h>

#define MQ_NAME "/mq_naveSimulador"
#define MUTEX_CONT_C "/sem_mutex_contC"
#define SHM_MAPA "/shm_estado"
#define MUTEX_MAPA "/sem_mutex_mapa"
#define SHM_TUB "/shm_tuberias"

	mqd_t colaSimulador;/*!<  Cola de mensajes, para la comunicacion  entre naves-simulador*/
	Mensaje msg;	/*!<  Variable que apunta a la estructura Mensaje del simulador.*/

	Tuberia *tuberia;
	int memoriaTuberia; /*!<  Memoria compartida que almacena los descriptores de fichero de los diferentes pipes tanto de los jefes como de las naves.*/
	sem_t *mutexMapa = NULL; /*!< Este semaforo controla la entrada de naves en la memoria compartida
	                            del mapa, para que solo puedan consultarlo de uno en uno*/
  int memoriaEstado;/*!< Memoria compartida para el mapa, la usaremos para poder realizar la accion de MOVER a una posicioón aleatoria*/
	tipo_mapa *mapa;
  int equipo; /*!< Variable global que almacena el numero de equipo de la nave, el cual es dado por argumento */
  int nNave;/*!<Variable global que almacena el numero de nave de las naves del equipo, el cual es dado por argumento*/

	sem_t *mutexContC = NULL;/*!<Semaforo que se encarga de controlar el uso del contadorCola*/
	struct sigaction act2;/*!< Estructura para la captura de señales, en este caso la de SIGTERM*/


/**
* @brief Funcion que se encarga de cerrar todos los recursos de la simulacion.
* @return void libera los recursos y finaliza
*/
void liberar(){

 	munmap(tuberia, sizeof(Tuberia));
 	close(memoriaTuberia);

 	sem_close(mutexContC);

 	sem_close(mutexMapa);

 	close(memoriaEstado);
 	munmap(mapa, sizeof(tipo_mapa));

 	mq_close(colaSimulador);
}

/**
* @brief Funcion que se encarga de manejar la señal SIGTERM, enviada por el proceso simulador cuando ha habido un
*  equipo ganador.
* @return void, termina la ejecucion de una nave
*/
void manejadorSIGTERM() {
	printf("\nNave %d/%d: RECIBIDO SIGTERM\n", equipo, nNave);
	liberar();
	exit(EXIT_SUCCESS);
}

/**
* @brief Funcion principal del proceso nave, se encargara de ejecutar todas las acciones una vez recibida la accion por la tuberia.
* @param argc: int, argv: char const
* @return devuelve EXIT_SUCCESS si todo ha ido correctamente
*/
int main(int argc, char const *argv[]) {
	Orden orden; /*Variable que apunta a la estructura Orden, en el cual se almacena la orden leida por la tuberia*/

	equipo = atoi(argv[1]);
	nNave = atoi(argv[2]);
	printf("\n[Nave %d/%d]: ARRANCADA\n", equipo, nNave);

	tipo_nave nave;

	act2.sa_handler = SIG_IGN;
	 sigemptyset(&(act2.sa_mask));
	 sigaddset(&(act2.sa_mask), SIGALRM);
	 act2.sa_flags = 0;

	if (sigaction(SIGINT, &act2, NULL) < 0) {
		perror("Error al capturar senyal de sigterm en nave\n");
		exit(EXIT_FAILURE);
	}

	///Abrimos los recursos creados en el simulador para poder usarlos en la nave

	if ((mutexContC = sem_open(MUTEX_CONT_C, 0)) == SEM_FAILED){
		perror("Error al abrir semaforo de contador de cola en las naves\n");
		return EXIT_FAILURE;
	}

	if ((mutexMapa = sem_open(MUTEX_MAPA, 0)) == SEM_FAILED){
		perror("Error al abrir semaforo de mutex mapa en las naves\n");
		liberar();
		return EXIT_FAILURE;
	}

	printf("[Nave %d/%d]: gestionando SHM de las tuberias\n", equipo, nNave);
	memoriaTuberia = shm_open(SHM_TUB, O_RDWR,  0);
	if(memoriaTuberia == -1) {
		perror("Error al crear memoria compartida tuberia nave\n");
		liberar();
		return EXIT_FAILURE;
	}

	tuberia =(Tuberia*) mmap(NULL, sizeof(Tuberia), PROT_READ | PROT_WRITE, MAP_SHARED, memoriaTuberia, 0);
	if(tuberia == MAP_FAILED) {
		perror("Error al hacer el mapeo de la tuberia en la nave\n");
		liberar();
		return EXIT_FAILURE;
	}

	printf("[Nave %d/%d]: gestionando SHM del mapa\n", equipo, nNave);
	memoriaEstado = shm_open(SHM_MAPA, O_RDWR,  0);
	if(memoriaEstado == -1) {
		perror("Error al crear memoria compartida mapa en nave\n");
		liberar();
		return EXIT_FAILURE;
	}

	mapa =(tipo_mapa*) mmap(NULL, sizeof(tipo_mapa), PROT_READ | PROT_WRITE, MAP_SHARED, memoriaEstado, 0);
	if(mapa == MAP_FAILED) {
		perror("Error al hacer el mapeo del mapa en la nave\n");
		liberar();
		return EXIT_FAILURE;
	}

	printf("[Nave %d/%d]: gestionando cola\n", equipo, nNave);
	colaSimulador = mq_open(MQ_NAME, O_WRONLY, S_IWUSR, NULL);
	if(colaSimulador == (mqd_t)-1) {
		perror ("Error al crear la cola de comunicacion entre naves-simulador\n");
		liberar();
		return EXIT_FAILURE;
	}

	srand(getpid()*time(NULL));

	///Inicializamos el contadorCola para contabilizar el numero de naves que van a enviar información al simulador
	sem_wait(mutexContC);
		 mapa->contadorCola = 0;
	sem_post(mutexContC);

	/*Indexar en el array de tuberias: FORMULA -->  i = ( 3*(equipo + 1) ) + nNave*/

	///Bucle en el que vamos a procesar las acciones recibidas
	while(1){
		sem_wait(mutexMapa);
		nave = mapa->info_naves[equipo][nNave];
		sem_post(mutexMapa);

		/*Comprobamos si la nave esta muerta, para que no pueda realizar ni procesar ninguna accion*/
		if (nave.viva == true){
			close(tuberia->fd[( N_EQUIPOS*(equipo + 1) ) + nNave][1]);
			read(tuberia->fd[( N_EQUIPOS*(equipo + 1) ) + nNave][0], &orden, sizeof(Orden));

			int flag = 0;

			/*Una vez leida la accion enviada por la tuberia, decidimos actuar dependiendo de si nos ha llegado DESTRUIR, ATACAR o MOVER*/
			switch (orden.accion){

				/*Si la accion recibida es DESTRUIR, entonces la nave realizara un exit, para terminar su programa,
				  sera el simulador el encargado de eliminar su simbolo en el mapa*/
				case DESTRUIR:
					printf("\n[Nave %d/%d]: ACCION DESTRUIR recibida\n", equipo, nNave);
					liberar();
					exit(EXIT_SUCCESS);
					break;
				/*Si la accion recibida es ATACAR, entonces la nave creara un numero aleatorio para elegir al equipo y la nave a la que atacar.
				  Una vez hecho esto, mandara la informacion necesaria al simulador para que realice la ejecucion necesaria*/
				case ATACAR:
					printf("\n[Nave %d/%d]: ACCION ATACAR recibida\n", equipo, nNave);
					int equipoDest;
					int naveDest;

					/*Generamos aleatorios hasta que toque un equipo con naves vivas*/
					while(flag == 0){
						equipoDest = (rand() % (N_EQUIPOS));
						sem_wait(mutexMapa);
						if (mapa->num_naves[equipoDest] != 0 && equipoDest != equipo){
							flag = 1;
						}
						sem_post(mutexMapa);
					}

					flag = 0;

					sem_wait(mutexMapa);

					/*Generamos aleatorios hasta que toque una nave viva dentro de ese equipo*/
					while (flag == 0){
						naveDest = (rand() % (N_NAVES));
						if (mapa->info_naves[equipoDest][naveDest].viva == true){
							flag = 1;
						}
					}

					msg.accion = ATACAR;
					msg.equipoOrigen = equipo;
					msg.naveOrigen = nNave;
					msg.naveDest = naveDest;
					msg.equipoDest = equipoDest;
					msg.destY = mapa->info_naves[equipoDest][naveDest].posy;
					msg.destX = mapa->info_naves[equipoDest][naveDest].posx;
					msg.origenY = mapa->info_naves[equipo][nNave].posy;
					msg.origenX = mapa->info_naves[equipo][nNave].posx;
					sem_post(mutexMapa);


					if (mq_send(colaSimulador, (char *)&msg, sizeof(msg), 1) == -1){
						perror("\nError al enviar mensaje al simulador con cola\n");
						exit(EXIT_FAILURE);
					}

					sem_wait(mutexContC);
					(mapa->contadorCola)++;
					sem_post(mutexContC);

					break;

				/*Si la accion recibida es MOVER, elegimos un numero aleatorio para elegir uno de los cuatro casos posibles,
				  y realizamos el respectivo control de errores */
				case MOVER:
					printf("\n[Nave %d/%d]: ACCION MOVER recibida\n", equipo, nNave);
					msg.accion = MOVER;
					msg.naveOrigen = nNave;
					msg.equipoOrigen = equipo;
					msg.origenY = mapa->info_naves[equipo][nNave].posy;
					msg.origenX = mapa->info_naves[equipo][nNave].posx;

					while (flag == 0){
						sem_wait(mutexMapa);
						int dest = (rand() %  (4)) + 1;

						switch(dest){
							/*Caso en el que el numero aleatorio sea 1, entonces se va a mover al NORTE y por lo tanto hay que comprobar
							  si esa casilla no esta vacia o que la posicion a la que se quiere acceder no se sale fuera del rango del mapa*/
							case 1:
								if (nave.posx-1 < 0){ //Si la posicion x de la nave es 0, se va a mover hacia el sur una posicion
									msg.destX = nave.posx+1;
									msg.destY = nave.posy;

								} else { // Y sino se va a mover a su casilla correspondiente
									msg.destX = nave.posx-1;
									msg.destY = nave.posy;
								}

								if (mapa_is_casilla_vacia(mapa, msg.destY, msg.destX) == true){
									flag = 1;
								}

								break;

							case 2: /*En el caso de que el numero aleatorio sea 2, nos moveremos al ESTE y realizaremos el respectivo control de errores*/
								if (nave.posy+1 >= MAPA_MAXY){
									msg.destY = nave.posy-1;
									msg.destX = nave.posx;

								} else {
									msg.destY = nave.posy+1;
									msg.destX = nave.posx;
								}

								if (mapa_is_casilla_vacia(mapa, msg.destY, msg.destX) == true){
									flag = 1;
								}

								break;

							case 3:/*En el caso de que salga 3, moveremos a la nave una posicion al SUR, donde realizamos el mismo control de errores*/
								if (nave.posx+1 >= MAPA_MAXX){
									msg.destX = nave.posx-1;
									msg.destY = nave.posy;

								} else {
									msg.destX = nave.posx+1;
									msg.destY = nave.posy;
								}

								if (mapa_is_casilla_vacia(mapa, msg.destY, msg.destX) == true){
									flag = 1;
								}

								break;

							case 4:/*En el caso de que llegue el numero 4, nos moveremos hacia el OESTE, donde volvemos a controlar que la casilla no este vacia,
											y que la posicion a la que se quiere acceder no se sale fuera del rango del mapa*/
								if (nave.posy-1 < 0){
									msg.destY = nave.posy+1;
									msg.destX = nave.posx;

								} else {
									msg.destY = nave.posy-1;
									msg.destX = nave.posx;
								}

								if (mapa_is_casilla_vacia(mapa, msg.destY, msg.destX) == true){
									flag = 1;
								}

								break;

							default:
								break;
						}

						sem_post(mutexMapa);
					}

					/*Se envia el respectivo mensaje dependiendo de la accion y del resultado de los numeros aleatorios*/
					if (mq_send(colaSimulador, (char *)&msg, sizeof(msg), 1) == -1){
						perror("Error al enviar mensaje de MOVER al simulador con cola\n");
						exit(EXIT_FAILURE);
					}
					/*Controlamos el acceso al contadorCola encargado de controlar los mensajes que se envian por la cola*/
					sem_wait(mutexContC);
					(mapa->contadorCola)++;
					sem_post(mutexContC);

					break;

				default:
						break;
			}
		}
	}

  exit(EXIT_SUCCESS);
}

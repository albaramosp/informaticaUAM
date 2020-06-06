/**
 * @brief Programa que gestiona el funcionamiento principal del proyecto. Se encarga de gestionar los turnos, detectar el fin de
 * la simulacion y procesar las acciones recibidas de las naves.
 * @file simulador.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 11-04-2019
 */
#include <fcntl.h>
#include <errno.h>
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
#include <time.h>
#include <mapa.h>

#define NUM 100

#define MQ_NAME "/mq_naveSimulador"
#define SHM_MAPA "/shm_estado"
#define SHM_TUB "/shm_tuberias"
#define SHM_CONTC "/shm_contCola"
#define MUTEX_CONT_C "/sem_mutex_contC"
#define MUTEX_MAPA "/sem_mutex_mapa"
#define SEM_JEFES "/sem_jefes"
#define SEM_MONITOR "/sem_monitor"


/**Recursos compartidos*/
int memoriaEstado; /*!<Memoria compartida para guardar estado del mapa y de la simulacion*/
tipo_mapa *mapa;

tipo_nave naves[N_EQUIPOS][N_NAVES]; /*!<El simulador actualizara las naves, segun las acciones recibidas.*/

sem_t *mutexContC = NULL; /*!<Semaforo que controla el acceso al contador de los mensajes enviados por la cola de mensajes*/
sem_t *mutexMapa = NULL; /*!<Semaforo que controla el acceso al mapa*/
sem_t *sem_jefes = NULL; /*!<Permitira que el simulador no empiece a enviar turnos ni nada hasta que no este todo creado*/
sem_t *sem_monitor = NULL; /*!<Permitira que el monitor espere a que el simulador tenga todo creado*/

Tuberia *tuberias; /*!<Tuberias tanto para simulador-jefe como para jefe-nave*/
int memoriaTuberias;

mqd_t colaSimulador; /*!<Cola de mensajes, para comunicacion naves-simulador*/
struct mq_attr attributes;

struct sigaction act; /*!<Manejador de la senyal de alarma para turnos*/
struct sigaction act2; /*!<Manejador de la senyal de Ctrl+C y SIGTERM*/

int flag = 0; /*!<Controla cuando lanzar nuevas alarmas de turno*/

Mensaje msg; /*!<Mensajes recibidos de las naves*/
Orden orden; /*!<Mensajes que se envian a los jefes*/

/**
 * @brief Funcion para liberar los recursos.
 * @return void
 */
void liberar(){
	sem_close(sem_jefes);
 	sem_unlink(SEM_JEFES);

 	sem_close(sem_monitor);
 	sem_unlink(SEM_MONITOR);

 	munmap(tuberias, sizeof(Tuberia));
 	close(memoriaTuberias);
 	shm_unlink(SHM_TUB);

 	sem_close(mutexContC);
 	sem_unlink(MUTEX_CONT_C);

 	sem_close(mutexMapa);
 	sem_unlink(MUTEX_MAPA);

 	close(memoriaEstado);
 	munmap(mapa, sizeof(tipo_mapa));
 	shm_unlink(SHM_MAPA);

 	mq_close(colaSimulador);
 	mq_unlink(MQ_NAME);
}

/**
 * @brief Manejador de la senyal de Ctrl+C y de SIGTERM. En el se liberan los recursos y se avisa del final
 * de la simulacion a los jefes.
 * @return void
 */
void manejadorFinal() {
	int i;
  Orden orden;
  orden.accion = FIN;

	act.sa_handler = SIG_IGN; /*!<Ignorar alarmas que puedan llegar despues de esto*/
	sigemptyset(&(act.sa_mask));
	act.sa_flags = 0;

	 if (sigaction(SIGALRM, &act, NULL) < 0) {
	   perror("Error al capturar senyal de alarma tras setear nuevo manejador");
	   exit(EXIT_FAILURE);
	 }

	printf("Simulador: entrando a manejador final\n");

  for (i = 0; i < N_EQUIPOS; i++){
 	 printf("Simulador: envia la señal al jefe %d\n", i);
 	 close(tuberias->fd[i][0]);
 	 write (tuberias->fd[i][1], &orden, sizeof(Orden));
  }


	printf("Simulador: libera recursos\n");
 	liberar();

 	printf("Simulador: esperando a finalizacion de los jefes\n");
 	for (i = 0; i < N_EQUIPOS; i++){
 		wait(NULL);
 	}

	printf("Simulador: termina\n");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Manejador de la senyal de alarma para turnos, que cambia el valor del flag para indicar que
 * se debe mandar una nueva alarma.
 * @return void
 */
void manejadorTurno() {
	printf("\n\nSimulador: Nuevo TURNO\n\n");
	flag = 1;
}


/**Proceso simulador*/
int main(int argc, char const *argv[]) {
	int i, j, x, y, cont = 0;
	pid_t pid;

	srand(time(0)); ///Semilla para generar el numero aleatorio

	printf("Simulador: inicializando recursos\n");

	 mutexContC = sem_open(MUTEX_CONT_C, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
	 if (mutexContC == SEM_FAILED) {
		 perror("Error al crear semaforo mutex lectura de contaador cola en el simulador");
		 return EXIT_FAILURE;
	 }

	/**Zona de memoria para compartir todas las tuberias*/
	memoriaTuberias = shm_open(SHM_TUB, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if(memoriaTuberias == -1) {
		perror("Error al crear memoria compartida tuberias");
 		liberar();
		return EXIT_FAILURE;
	}

	if(ftruncate(memoriaTuberias, sizeof(Tuberia)) == -1) {
		perror("Error al hacer truncate tuberia");
 		liberar();
		return EXIT_FAILURE;
	}

	tuberias =(Tuberia*) mmap(NULL, sizeof(Tuberia), PROT_READ | PROT_WRITE, MAP_SHARED, memoriaTuberias, 0);
	if(tuberias == MAP_FAILED) {
		perror("Error al hacer el mapeo tuberias");
 		liberar();
		return EXIT_FAILURE;
	}

	/**Inicializacion de tuberias*/
	for(i = 0; i < N_EQUIPOS + (N_NAVES*N_EQUIPOS); i++){
		if(pipe(tuberias->fd[i]) == -1) {
			perror("Error creando tuberia simulador-jefe\n");
 			liberar();
			exit(EXIT_FAILURE);
		}
	}

	sem_jefes = sem_open(SEM_JEFES, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
	if (sem_jefes == SEM_FAILED) {
		perror("Error al crear semaforo de los jefes listos en el simulador");
 		liberar();
		return EXIT_FAILURE;
	}

	mutexMapa = sem_open(MUTEX_MAPA, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
	if (mutexMapa == SEM_FAILED) {
		perror("Error al crear semaforo mutex lectura de mapa en el simulador");
 		liberar();
		return EXIT_FAILURE;
	}

	/**Memoria compartida para guardar estado del mapa y de la simulacion*/
	memoriaEstado = shm_open(SHM_MAPA, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if(memoriaEstado == -1) {
		perror("Error al crear memoria compartida");
 		liberar();
		return EXIT_FAILURE;
	}

	if(ftruncate(memoriaEstado, sizeof(tipo_mapa)) == -1) {
		perror("Error al hacer truncate");
 		liberar();
		return EXIT_FAILURE;
	}

	mapa = (tipo_mapa *)mmap(NULL, sizeof(tipo_mapa), PROT_READ | PROT_WRITE, MAP_SHARED, memoriaEstado, 0);
	if(mapa == MAP_FAILED) {
		perror("Error al hacer el mapeo");
 		liberar();
		return EXIT_FAILURE;
	}

	/**Cola de mensajes nave-simulador*/
	 attributes.mq_flags = 0;
	 attributes.mq_maxmsg = 10;
	 attributes.mq_curmsgs = 0;
	 attributes.mq_msgsize = sizeof(Mensaje);

	colaSimulador = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes);
	if(colaSimulador == (mqd_t)-1) {
		perror ("Error al crear la cola de comunicacion entre naves-simulador\n");
 		liberar();
		return EXIT_FAILURE;
	}

	sem_monitor = sem_open(SEM_MONITOR, O_CREAT, S_IRUSR | S_IWUSR, 0);
	if (sem_monitor == SEM_FAILED) {
		perror("Error al crear semaforo de aviso al monitor en el simulador");
 		liberar();
		return EXIT_FAILURE;
	}

	printf("Simulador: recursos inicializados\n");

	printf("Simulador: inicializando mapa y naves\n");
	/**rellenamos primero todo el mapa con casillas vacias*/
	for (x = 0; x < MAPA_MAXX; x++){
		for (y = 0; y < MAPA_MAXY; y++){
			mapa_clean_casilla(mapa, y, x);
		}
	}

	for (i = 0; i < N_EQUIPOS; i++){///Creamos las naves para cada equipo y las posicionamos en el mapa
		mapa_set_num_naves(mapa, i, N_NAVES); /**al principio todas las naves del equipo i-esimo estan vivas*/

		/**colocamos las naves en el mapa*/
		for (j = 0; j < N_NAVES; j++){
			do {
				x = (rand() % (MAPA_MAXX +1));///numero de filas
				y = (rand() % (MAPA_MAXY +1));///numero de columnas
			}	while (mapa_is_casilla_vacia(mapa, y, x) == false);

			tipo_nave nave;
			nave.vida = VIDA_MAX;
			nave.posx = x;
			nave.posy = y;
			nave.equipo = i;
			nave.numNave = j;
			nave.viva = true;

			naves[i][j] = nave;

			if (mapa_set_nave(mapa, nave) == -1){ /**anyadir cada nave inicializada al equipo*/
				perror("Error al anyadir nave al mapa");
				exit(EXIT_FAILURE);
			}
		}
	}

	sem_post(sem_monitor);

	printf("Simulador: mapa y naves inicializados\n");

	/**Inicializa el contador de mensajes en la cola a 0*/
	sem_wait(mutexContC);
		 mapa->contadorCola = 0;
		 mapa->contadorPerdedores = 0;
	sem_post(mutexContC);

	/**Creacion de procesos jefe*/
	for (i = 0; i < N_EQUIPOS; i++){
		pid = fork();

		if (pid < 0){
			perror("Error al hacer fork");
 			liberar();
			return EXIT_FAILURE;
		}

		else if (pid == 0){
			char texto[5];
			sprintf(texto, "%d", cont);//Convertimos el contador a una cadena
			argv[1] = texto;//Lo igualamos a un const char* para pasarselo a la funcion execlp

			if (execlp("./jefe", "./jefe", argv[1], (char *)NULL) == -1){
				perror("Error al hacer el exec en simulador para crear jefe");
 				liberar();
			}
		}

		else {
			cont++;

			if (cont == N_EQUIPOS){ //Espera a que todos los jefes esten creados
				/**Manejadores, se declaran solo en el simulador para que no se hereden*/
				act2.sa_handler = manejadorFinal;
			  sigemptyset(&(act2.sa_mask));
			  sigaddset(&(act2.sa_mask), SIGALRM); /*Si llega SIGINT o SIGTERM, ignorar alarmas que pudiesen interrumpir*/
			  act2.sa_flags = 0;

				act.sa_handler = manejadorTurno;
				sigemptyset(&(act.sa_mask));
				act.sa_flags = 0;

				if (sigaction(SIGINT, &act2, NULL) < 0) {
					 perror("Error al capturar senyal de Ctrl+C");
						 liberar();
					 exit(EXIT_FAILURE);
				}

				if (sigaction(SIGALRM, &act, NULL) < 0) {
					perror("Error al capturar senyal de alarma");
 					liberar();
					exit(EXIT_FAILURE);
				}

				if (sigaction(SIGTERM, &act2, NULL) < 0) {
					perror("Error al capturar senyal de SIGTERM");
 					liberar();
					exit(EXIT_FAILURE);
				}

				sem_wait(sem_jefes); //Hasta que no este todo creado no vamos a empezar
					printf("Simulador: Crea el primer TURNO\n");

					if(alarm(TURNO_SECS))
						perror("Error en la alarma existente en simulador\n");

					//(rand() %  (upper - lower + 1)) + lower;

					while(1){
						if (flag == 1){
							int i;
							Orden orden;
							orden.accion = TURNO;

							mapa_restore(mapa); /*Se dejan los simbolos de las naves nada mas, sin los de agua, tocado, etc.*/
							printf("Simulador: restaura mapa\n");

							if (mapa->contadorPerdedores == N_EQUIPOS-1){
								/*Buscamos al equipo ganador*/
								for (j = 0; j < N_EQUIPOS; j++){
									if (mapa->num_naves[j] != 0){
										printf("******* Simulador: ha habido un GANADOR: el equipo %d *******\n", j);
										kill(0, SIGTERM);
									}
								}
							}

							for (i = 0; i < N_EQUIPOS; i++){
								printf("Simulador: va a escribir la señal de TURNO en la tuberia del jefe %d\n", i);
								close(tuberias->fd[i][0]);
								write (tuberias->fd[i][1], &orden, sizeof(Orden));
							}

							flag = 0;

							if(alarm(TURNO_SECS))
								perror("Error en la alarma existente en simulador\n");
						}

						/*Protegemos las llamadas bloqueantes frente a interrupciones de señal de alarma*/
						while(sem_wait(mutexContC) == -1){
							if (errno != EINTR){
								liberar();
								exit(EXIT_FAILURE);
							}
						}

						/*Solo lee de la cola si hay mensajes, ya que la llamada a receive es bloqueante*/
						if ((mapa->contadorCola) != 0){
							while(mq_receive(colaSimulador, (char *)&msg, sizeof(msg), NULL) == -1){
								if (errno != EINTR){
									liberar();
									exit(EXIT_FAILURE);
								}
							}


							(mapa->contadorCola)--; /*Ha leido un mensaje de la cola, decrementa el contador*/
							sem_post(mutexContC);

							if (naves[msg.equipoOrigen][msg.naveOrigen].viva == true){
								tipo_casilla o = mapa_get_casilla(mapa, msg.origenY, msg.origenX);
								tipo_casilla c = mapa_get_casilla(mapa, msg.destY, msg.destX);


								switch (msg.accion){
									case MOVER:
										printf("Simulador: recibe ACCION MOVER [%c %d] de (x=%d, y=%d) a (x=%d, y=%d)\n",o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);

										while(sem_wait(mutexMapa) == -1){
											if (errno != EINTR){
												liberar();
												exit(EXIT_FAILURE);
											}
										}

										/*Comprueba si la accion puede realizarse, si la casilla de destino esta vacia*/
										if (mapa_is_casilla_vacia(mapa, msg.destY, msg.destX) == true){
											mapa_clean_casilla(mapa, msg.origenY, msg.origenX);

											naves[msg.equipoOrigen][msg.naveOrigen].posy = msg.destY;
											naves[msg.equipoOrigen][msg.naveOrigen].posx = msg.destX;

											usleep(100100); /*Esperamos para que se vea mejor el simbolo*/
											mapa_set_nave(mapa, naves[msg.equipoOrigen][msg.naveOrigen]);
											printf("Simulador: ---> ACCION MOVER realizada [%c %d] de (x=%d, y=%d) a (x=%d, y=%d)\n",o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);

										} else {
											printf("Simulador: ACCION MOVER no realizada [%c %d] de (x=%d, y=%d) a (x=%d, y=%d): casilla ocupada\n", o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);
										}

										sem_post(mutexMapa);
										break;

									case ATACAR:
										printf("Simulador: ACCION ATACAR recibida desde [%c %d] de (x=%d, y=%d) a (x=%d, y=%d)\n", o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);

										/*Protegemos las llamadas bloqueantes frente a interrupciones de señal de alarma*/
										while(sem_wait(mutexMapa) == -1){
											if (errno != EINTR){
												liberar();
												exit(EXIT_FAILURE);
											}
										}

										/*Comprueba si la accion puede realizarse, primero si el objetivo no esta fuera de rango*/
										if (mapa_get_distancia(mapa, msg.origenY, msg.origenX, msg.destY, msg.destX) <= ATAQUE_ALCANCE){
											/*Comprueba si el objetivo no esta vacio o es del mismo equipo*/
											if (c.equipo != msg.equipoOrigen && o.simbolo != SYMB_VACIO){
												if (mapa_is_casilla_vacia(mapa, msg.destY, msg.destX) == true){
													printf("------------------->SE HA ATACADO UN ESPACIO VACIO\n");
													mapa_send_misil(mapa, msg.origenY, msg.origenX, msg.destY, msg.destX);
													mapa_set_symbol(mapa, msg.destY, msg.destX, SYMB_AGUA);

												} else {
													/*Comprueba si el objetivo sigue vivo*/
													if (naves[msg.equipoDest][msg.naveDest].viva == true){
														naves[msg.equipoDest][msg.naveDest].vida--;

														mapa_send_misil(mapa, msg.origenY, msg.origenX, msg.destY, msg.destX);

														/*Si la nave objetivo ha perdido toda su vida*/
														if (naves[msg.equipoDest][msg.naveDest].vida <= 0){
															mapa_set_symbol(mapa, msg.destY, msg.destX, SYMB_DESTRUIDO);

															/*Manda destruir como orden al jefe correspondiente*/
															orden.accion = DESTRUIR;
															orden.dest = msg.naveDest;
															close(tuberias->fd[msg.equipoDest][0]);
															write (tuberias->fd[msg.equipoDest][1], &orden, sizeof(Orden));

															/*Actualiza informacion del mapa*/
															naves[msg.equipoDest][msg.naveDest].viva = false;
															mapa->num_naves[msg.equipoDest]--;
															printf("Simulador: [Nave %d/%d] MUERTA\n",  msg.equipoDest, msg.naveDest);

															if (mapa->num_naves[msg.equipoDest] == 0){
																mapa->contadorPerdedores++;
																printf("--------------------------------Simulador: otro perdedor--------------------------------\n");
															}

															/*Si la nave aun le queda vida*/
														} else {
															mapa_set_symbol(mapa, msg.destY, msg.destX, SYMB_TOCADO);
															printf("------------------->SE HA TOCADO UNA NAVE\n");
														}
													}
												}

												usleep(100100); /*Para ver mejor el simbolo de la nave*/
												mapa_set_nave(mapa, naves[msg.equipoDest][msg.naveDest]);

												printf("Simulador: ACCION ATACAR realizada desde [%c %d] de (x=%d, y=%d) a (x=%d, y=%d)\n", o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);
											}else{
												printf("Simulador: ACCION ATACAR no realizada desde [%c %d] de (x=%d, y=%d) a (x=%d, y=%d): casilla origen vacia\n", o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);
											}

										} else {
											printf("Simulador: ACCION ATACAR no realizada desde [%c %d] de (x=%d, y=%d) a (x=%d, y=%d): fuera de rango\n", o.simbolo, msg.naveOrigen, msg.origenY, msg.origenX, msg.destY, msg.destX);
										}

										sem_post(mutexMapa);
										break;

									default:
										break;
								}

								usleep(100000);
							} else {
								sem_post(mutexContC);
							}
						} else {
							sem_post(mutexContC);
						}

					} //while
			}
		}
	}

	/*Esperar a que acaben los jefes*/
	for (i = 0; i < N_EQUIPOS; i++){
		wait(NULL);
	}

    exit(EXIT_SUCCESS);
}

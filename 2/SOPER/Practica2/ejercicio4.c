/**
 * @brief Programa que simula una carrera entre NUM_PROC procesos hijos que compiten por ver quien captura antes ua senyal.
 * @file ejercicio4.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 06-03-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define NUM_PROC 3

/* manejador_SIGUSR1: presenta un mensaje por pantalla.*/
void manejador_SIGUSR1(int sig) {
  printf("\tEl manejador avisa: la señal SIGUSR1 ha llegado\n");
  fflush(stdout);
}

/* manejador_SIGUSR2: presenta un mensaje por pantalla. */
void manejador_SIGUSR2(int sig) {
  printf("\tEl manejador avisa: la señal SIGUSR2 ha llegado\n");
  fflush(stdout);
}


int main(void){
	pid_t pid;
	int i;

	pid = fork();

	if(pid <  0){
		perror("Error al emplear fork en el padre\n");
		exit(EXIT_FAILURE);
	}

  //Proceso gestor
	else if(pid ==  0){
    for(i = 0; i < NUM_PROC; i++){
      pid = fork();
      if(pid <  0){
        perror("Error al emplear fork en el gestor\n");
        exit(EXIT_FAILURE);
      }

      if(pid == 0){
        //los hijos deben mandar la señal de que estan listos
        printf("Soy el hijo de la ejecucion %d y voy a enviar mi señal al gestor \n", i);
        if (kill(getppid(), SIGUSR2)){
          perror("Ha habido error en kill. Fin del programa");
          return EXIT_FAILURE;
        }

        //los hijos esperan a la señal del padre, y cuando les llega, imprimen un mensaje y terminan
        struct sigaction act;
        act.sa_handler = manejador_SIGUSR1;
        sigemptyset(&(act.sa_mask));
        act.sa_flags = SA_RESETHAND;

        if (sigaction(SIGUSR1, &act, NULL) < 0) {
          perror("SIGUSR1 en el hijo");
          exit(EXIT_FAILURE);
        }

        pause();
        printf("Soy el hijo de la ejecucion %d y el padre me ha enviado ya la señal SIGUSR1 \n", i);
        exit(EXIT_SUCCESS);
      }

      else{
        //el gestor espera a la señal de los hijos
        struct sigaction act;
        act.sa_handler = manejador_SIGUSR2;
        sigemptyset(&(act.sa_mask));
        act.sa_flags = SA_RESETHAND;

        if (sigaction(SIGUSR2, &act, NULL) < 0) {
          perror("SIGUSR2 en el gestor");
          exit(EXIT_FAILURE);
        }

        pause();
        printf("Soy el gestor y ya he recibido la señal de mi hijo %d \n", i);
      }
    }

    //fuera del bucle
    //el gestor ya ha creado los hijos, que estan esperando a la señal del padre.
    //el gestor manda la señal al padre de que esta todo listo
    if (kill(getppid(), SIGUSR2)){
      perror("Ha habido error en kill. Fin del programa");
      return EXIT_FAILURE;
    }

    //el gestor,cuando reciba la señal del padre, va a hacer los wait() para todos sus hijos, y acaba
    struct sigaction act;
    act.sa_handler = manejador_SIGUSR1;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = SA_RESETHAND;

    if (sigaction(SIGUSR1, &act, NULL) < 0) {
      perror("SIGUSR1 en el gestor");
      exit(EXIT_FAILURE);
    }

    pause();
    printf("Soy el gestor y ya he recibido la señal SIGUSR1 de mi padre. Voy a esperar a todos mis hijos \n");
    for(i = 0; i < NUM_PROC; i++){
      wait(NULL);
    }

    printf("\nFin gestor\n");
    exit(EXIT_SUCCESS);
	}

  //Proceso padre
  else if (pid > 0){
    //el padre espera a la señal del gestor de que todos los hijos estan listos
    struct sigaction act;
    act.sa_handler = manejador_SIGUSR2;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = SA_RESETHAND;

    if (sigaction(SIGUSR2, &act, NULL) < 0) {
      perror("SIGUSR2 en el padre");
      exit(EXIT_FAILURE);
    }

    pause();
    printf("\n\n******SOY EL PADRE Y HE RECIBIDO LA SEÑAL DEL GESTOR***********\n\n");

    //el padre manda la señal a todos los del grupo, incluido a si mismo
    printf("\nSoy el padre y voy a enviar la señal SIGUSR1 a todo el grupo, y a mi mismo\n\n");

    //el padre espera a la señal que ha enviado al grupo y a si mismo, y decide ignorarla
    act.sa_handler = SIG_IGN; //funcion que ignora la señal
    sigemptyset(&(act.sa_mask));
    act.sa_flags = SA_RESETHAND;

    if (sigaction(SIGUSR1, &act, NULL) < 0) {
      perror("SIGUSR1 en el padre");
      exit(EXIT_FAILURE);
    }

    if(kill(0, SIGUSR1)){
      perror("\n\nERROR AL ENVIAR A TODO EL GRUPO\n\n");
      return EXIT_FAILURE;
    }

    //no ponemos pause en el padre porque como hemos dicho que ignore la señal, se quedaria pillado en el pause porque ya la ignora
	}

  //finalmente, espera a su unico hijo (el gestor)
  printf("\n\nSoy el padre y voy a esperar al gestor\n\n");
  wait(NULL);

  printf("\n\nSoy el padre y ya voy a terminar\n\n");
  exit(EXIT_SUCCESS);
}

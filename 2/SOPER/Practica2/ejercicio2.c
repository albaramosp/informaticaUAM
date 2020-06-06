/**
 * @brief Programa donde se crean 4 hijos paralelos y cada uno imprime un mensaje, duerme, imprime otro mensaje y termina. 
 * El padre, mientras, espera un tiempo tras crear cada hijo y manda SIGTERM al hijo, para crear el siguiente a continuacion.
 * @file ejercicio2.c
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

#define NUM_PROC 4

int main(void)
{
	pid_t pid;
	int i;

	for(i = 0; i < NUM_PROC; i++){
		pid = fork();

		if(pid <  0){
			perror("Error al emplear fork\n");
			exit(EXIT_FAILURE);
		}


		else if(pid ==  0){
			printf("Soy el proceso hijo <%d> \n", getpid());
		  usleep (30000000) ;
		  printf("Soy el proceso hijo <%d> y ya me toca terminar.\n", getpid());
			exit(EXIT_SUCCESS);
		}

    else if (pid > 0){
      printf("Soy el padre de la iteracion %d y voy a dormir\n", i);
      usleep (5000000) ;
      printf("Soy el padre de la iteracion %d y voy a matar a mi hijo %d\n", i, pid);

			if (kill (pid, 15) == -1){ //sabemos que SIGTERM es la 15. Lo hemos dejado asi para comprobar que podemos utilizar tanto el nombre como el numero.
					perror("Error al hacer kill");
					break;
			}
    }
	}

  for(i = 0; i < NUM_PROC; i++){
    wait(NULL);
  }

	exit(EXIT_SUCCESS);
}

/**
 * @brief  El proceso padre crea NUM_PROC hijos que van terminando durante el bucle.
 * 		   Al final, el padre espera a la señal de terminación de todos sus hijos, para no dejar huérfanos.
 *
 * @file p1_exercise_3.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 07-02-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_PROC 3

/**
 * @author Alba Ramos
 * @author Andrea Salcedo
 * @brief  El proceso padre crea NUM_PROC hijos que van terminando durante el bucle.
 * 		   El padre imprime su id. El hijo imprime su id y el de su padre.
 * @param void
 * @return éxito o fallo de la operación
 */
int main(void)
{
	pid_t pid;
	int i;

	for(i = 0; i < NUM_PROC; i++){
		pid = fork();
		if(pid <  0){
			printf("Error al emplear fork\n");
			exit(EXIT_FAILURE);
		}

		else if(pid ==  0){
			printf("HIJO. ");
			printf("Id: %d. ", getpid());
			printf("Id del padre: %d\n", getppid());
			exit(EXIT_SUCCESS);
		}

		else if(pid >  0){
			printf("PADRE. Bucle numero %d. Id: %d\n", i, getpid());
		}
	}

	for(i = 0; i < NUM_PROC; i++){
		wait(NULL);
	}

	exit(EXIT_SUCCESS);
}

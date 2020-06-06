/**
 * @brief  El proceso padre crea un hijo que a su vez va creando otro hijo, hasta tener una escalera de NUM_PROC hijos.
 * 		   Cada padre, antes de acabar, espera a la señal de terminación de su hijo, para no dejar huérfanos.
 *
 * @file ejercicio4.c
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
 * @brief  El proceso padre crea un hijo y espera a su señal de terminación antes de salir.
 * 		   El hijo ahora realiza la misma operación, que se repite NUM_PROC veces.
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
		}

		else if(pid >  0){
			printf("PADRE. Bucle numero %d. Id: %d\n", i, getpid());
			wait(NULL);
			exit(EXIT_SUCCESS);
		}
	}

	exit(EXIT_SUCCESS);
}

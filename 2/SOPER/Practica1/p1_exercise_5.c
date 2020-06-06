/**
 * @brief El proceso padre reserva memoria para la variable sentence.
 * El hijo inicializa su variable sentence, la libera y acaba. Al final, el padre
 * espera la señal de terminación del hijo antes de imprimir su variable, liberarla y acabar.
 *
 * @file p1_exercise_5.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 11-02-2019
 */


#include <sys/types.h>
#include <sys/wait.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

/**
 * @author Alba Ramos
 * @author Andrea Salcedo
 * @brief En la función, el proceso padre reserva memoria para la variable sentence.
 * El hijo inicializa su variable sentence y el padre imprime la suya. Finalmente, se libera la memoria reservada en ambos.
 * @param void
 * @return éxito o fallo de la operación
 */
int  main ( void )
{
	pid_t pid;
	char * sentence = (char *)malloc(5 * sizeof (char));

	pid = fork();
	if (pid <  0  )
	{
		printf("Error al emplear fork\n");
		exit (EXIT_FAILURE);
	}
	else  if(pid ==  0)
	{
		strcpy(sentence, "hola");
		free(sentence);
		exit(EXIT_SUCCESS);
	}
	else
	{
		wait(NULL);
		//strcpy(sentence, "hola");
		printf("Padre: %s\n", sentence);
		free(sentence);
		exit(EXIT_SUCCESS);
	}
}

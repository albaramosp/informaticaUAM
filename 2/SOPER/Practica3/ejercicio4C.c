/**
 * @brief Programa que va leyendo los mensajes codificados de la segunda cola, y los muestra por pantalla
 * @file ejercicio4C.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 04-04-2019
 */

#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define TAMPAQUETE 2000

typedef struct {
	int longitud;
	char texto[TAMPAQUETE];
} Mensaje;

int main(int argc, char const *argv[]) { //Segundo arg: nombre Q2

	if (argc != 2){
		printf("Introducir en C nombre de segunda cola SOLAMENTE POR FAVOR GRACIAS ;)\n");
		return EXIT_FAILURE;
	}

	mqd_t queue;
	Mensaje msg;

	printf("Ejecutando C\n");

 /*Abrimos la cola que fue inicializada en el main*/
	queue = mq_open(argv[1], O_RDONLY, S_IRUSR, NULL);

	if(queue == (mqd_t)-1) {
		perror ("Error al abrir cola 2 en programa C\n");
		return EXIT_FAILURE;
	}

	printf("Mensaje final: ");

	/*Imprimimos los mensajes recibidos hasta recibir eof*/
	do {
		if(mq_receive(queue, (char *)&msg, sizeof(msg), NULL) == -1) {
			fprintf (stderr, "Error al recibir mensaje en programa C\n");
			mq_close(queue);
			return EXIT_FAILURE;
		}

		printf("%s", msg.texto);
	} while (msg.longitud != -1);

	printf("Finalizando C\n");

	mq_close(queue);
	return EXIT_SUCCESS;
}

/**
 * @brief Programa que va leyendo los mensajes de la primera cola, los codifica y los va enviando a la segunda, y envia al acabar un mensaje
 * especial de fin de fichero.
 * @file ejercicio4B.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 04-04-2019
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TAMPAQUETE 2000

typedef struct {
	int longitud;
	char texto[TAMPAQUETE];
} Mensaje;

int main(int argc, char const *argv[]) { //Segundo arg: nombre Q1, Tercero arg: nombre Q2

	Mensaje msg;
	int i;

	if (argc != 3){
		printf("Introducir en B nombre de colas SOLAMENTE POR FAVOR GRACIAS ;)\n");
		return EXIT_FAILURE;
	}

	mqd_t queue1, queue2;

	printf("Ejecutando B\n");

	queue1 = mq_open(argv[1], O_RDONLY, S_IRUSR, NULL);

	if(queue1 == (mqd_t)-1) {
		perror ("Error al abrir primera cola en B\n");
		return EXIT_FAILURE;
	}


	queue2 = mq_open(argv[2], O_WRONLY, S_IWUSR, NULL);

	if(queue2 == (mqd_t)-1) {
		fprintf (stderr, "Error al abrir segunda cola en B\n");
		mq_close(queue1);
		return EXIT_FAILURE;
	}

	printf("Colas de B abiertas\n");

	/*Leer primer mensaje por si acaso nos han pasado un fichero vacio*/
	if(mq_receive(queue1, (char *)&msg, sizeof(msg), NULL) == -1) {
		fprintf (stderr, "Error al recibir primer mensaje en B\n");

		if (errno == EINTR){
			printf("Es el error raroo \n");
		}

		mq_close(queue1);
		mq_close(queue2);
		return EXIT_FAILURE;
	}


	/*Leer los mensajes de la primera cola hasta recibir mensaje de fin de fichero*/
	while(msg.longitud != -1){


		/*Convertimos caracter a caracter*/
		for (i = 0; i < msg.longitud; i++){
			int ascii = (int)(*(msg.texto+i));

			/*Comprobamos si es mayuscula*/
			if (ascii >= 65 && ascii <= 90){
				/*Caso Z*/
				if (ascii == 90){
					ascii = 65;
				}
				else {
					ascii += 1;
				}

			}
			/*Comprobamos si es minuscula*/
			else if (ascii >= 97 && ascii <= 122){
				/*Caso z*/
				if (ascii == 122){
					ascii = 97;
				}
				else {
					ascii += 1;
				}
			}

			(*(msg.texto+i)) = (char)ascii;
		}

		/*Enviar mensaje */
		if (mq_send(queue2, (char *)&msg, sizeof(msg), 1) == -1){
			fprintf (stderr, "Error al mandar el mensaje transformado en B\n");
			mq_close(queue1);
			mq_close(queue2);
			return EXIT_FAILURE;
		}

		/*Leer siguiente*/
		if(mq_receive(queue1, (char *)&msg, sizeof(msg), NULL) == -1) {
			fprintf (stderr, "Error al recibir siguiente mensaje en B\n");
			mq_close(queue1);
			mq_close(queue2);
			return EXIT_FAILURE;
		}
	}


	/*Enviar mensaje vacio de fin de fichero*/
	msg.longitud = -1;
	strcpy(msg.texto, " ");
	if (mq_send(queue2, (char *)&msg, sizeof(msg), 1) == -1){
		fprintf (stderr, "Error al mandar el mensaje de fin de fichero en B\n");
		mq_close(queue1);
		mq_close(queue2);
		return EXIT_FAILURE;
	}

	printf("Finalizando B\n");

	mq_close(queue1);
	mq_close(queue2);
	return EXIT_SUCCESS;
}

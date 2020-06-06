/**
 * @brief Programa que va leyendo trozos de fichero del tamanyo especificado en la macro, y los va enviando como mensajes a la primera cola.
 * Se ha decidido enviar un mensaje especial cuando el fichero haya finalizado, con longitud -1
 * @file ejercicio4A.c
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

int main(int argc, char const *argv[]) { //Segundo arg: filename, Tercero arg: nombre Q1

	if (argc != 3){
		printf("Introducir en A nombre de fichero y nombre de primera cola SOLAMENTE POR FAVOR GRACIAS ;)\n");
		return EXIT_FAILURE;
	}

	printf("Ejecutando A\n");

	mqd_t queue;
	int fd;
	char *file = NULL;
	Mensaje msg;
	struct stat fileInfo;

	fd = open(argv[1], O_RDONLY); /*obtenemos el descriptor para el fichero recibido por argumento, modo lectura*/
	fstat(fd, &fileInfo); /*obtenemos toda la informacion del fichero, lo que nos interesa es st_size que es tamanyo en bytes*/

  /*con este descriptor y tamanyo en bytes, lo cargamos en nuestro string grande*/
	file = (char *)mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	printf("Mensaje original: %s\n", file);

 /*Abrimos la cola que fue inicializada en el main*/
	queue = mq_open(argv[2], O_WRONLY, S_IWUSR, NULL);

	if(queue == (mqd_t)-1) {
		fprintf (stderr, "Error al abrir cola en A\n");
		close(fd);
		unlink(argv[1]);
		munmap(file, fileInfo.st_size);
		return EXIT_FAILURE;
	}

	/*Calculamos cuantos paquetes de 2000B tenemos que enviar*/
	int cociente = fileInfo.st_size/TAMPAQUETE;
	int resto = fileInfo.st_size%TAMPAQUETE;
	int i = 0;

	for (i = 0; i < cociente; i++){
		memcpy(msg.texto, (file+TAMPAQUETE*i), TAMPAQUETE);
		msg.longitud = strlen(msg.texto);

		if (mq_send(queue, (char *)&msg, sizeof(msg), 1) == -1){
			fprintf (stderr, "Error al mandar paquete %d en A\n", i);
			munmap(file, fileInfo.st_size);
			mq_close(queue);
			close(fd);
			unlink(argv[1]);
			return EXIT_FAILURE;
		}

	}

	/*Si ha habido resto, enviamos ese ultimo paquete de menor tamanyo*/
	if (resto != 0){
		memcpy(msg.texto, (file+(TAMPAQUETE*i)), resto+1);
		msg.longitud = strlen(msg.texto);

		if (mq_send(queue, (char *)&msg, sizeof(msg), 1) == -1){
			fprintf (stderr, "Error al mandar el resto en A\n");
			munmap(file, fileInfo.st_size);
			mq_close(queue);
			close(fd);
			unlink(argv[1]);
			return EXIT_FAILURE;
		}

	}

	/*Informamos de fin de fichero*/
	msg.longitud = -1;
	strcpy(msg.texto, " ");
	if (mq_send(queue, (char *)&msg, sizeof(msg), 1) == -1){
		fprintf (stderr, "Error al mandar el fin de fichero en A\n");
		munmap(file, fileInfo.st_size);
		mq_close(queue);
		close(fd);
		unlink(argv[1]);
		return EXIT_FAILURE;
	}

	printf("Finalizando A\n");

	mq_close(queue);
	return EXIT_SUCCESS;
}

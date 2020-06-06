/**
 * @brief Programa que crea e inicializa las colas y llama a los otros tres (A, B y C) mediante llamadas a fork y exec
 * @file ejercicio4.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 04-04-2019
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define TAMPAQUETE 2000
#define N 3

typedef struct {
	int longitud;
	char texto[TAMPAQUETE];
} Mensaje;

int main(int argc, char const *argv[]) { //Segundo arg: filename, Tercero arg: nombre Q1, Cuarto arg: nombre cola 2

	if (argc != 4){
		printf("Introducir nombre de fichero y nombre de colas SOLAMENTE POR FAVOR GRACIAS ;)\n");
		return EXIT_FAILURE;
	}

	mqd_t queue1, queue2;
	int i;
	pid_t pid;
	struct mq_attr attributes;
	  attributes.mq_flags = 0;
	  attributes.mq_maxmsg = 10;
	  attributes.mq_curmsgs = 0;
	  attributes.mq_msgsize = sizeof(Mensaje);

	queue1 = mq_open(argv[2], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes);

	if(queue1 == (mqd_t)-1) {
		perror ("Error al abrir primera cola\n");
		return EXIT_FAILURE;
	}

	queue2 = mq_open(argv[3], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes);

	if(queue2 == (mqd_t)-1) {
		perror ("Error al abrir segunda cola\n");
		mq_close(queue1);
		mq_unlink(argv[2]);
		return EXIT_FAILURE;
	}

	/*Creacion y ejecucion de hijos*/
		pid = fork();
	   if (pid < 0) {
	     perror("fork");
	     exit(EXIT_FAILURE);
	   }

		 /*Hijo*/
	  if (pid == 0) {
			argv[0] = "./ejercicio4A";
		  //Importante enviar como primer argumento el nombre del programa, es lo que recibira el main del programa que se ejecute
			execlp(argv[0], argv[0],  argv[1], argv[2], (char *)NULL); //execlp mejor que execvp para evitar enviar un array entero
			perror("fallo en exec A");
			exit(EXIT_FAILURE);
		}

		else {
			pid = fork();

		   if (pid < 0) {
		     perror("fork");
		     exit(EXIT_FAILURE);
		   }

			 /*Hijo*/
			 if (pid == 0) {
				 argv[0] = "./ejercicio4B";
				 execlp(argv[0], argv[0], argv[2], argv[3], (char *)NULL);
				 perror("fallo en exec B");
				 mq_close(queue1);
				 mq_unlink(argv[2]);
				 mq_close(queue2);
				 mq_unlink(argv[3]);
		     exit(EXIT_FAILURE);
			 }
			 else {
				 pid = fork();

		 	   if (pid < 0) {
		 	     perror("fork");
				   mq_close(queue1);
					 mq_unlink(argv[2]);
					 mq_close(queue2);
					 mq_unlink(argv[3]);
		 	     exit(EXIT_FAILURE);
		 	   }

				 /*Hijo*/
				 if (pid == 0){
					 argv[0] = "./ejercicio4C";
					 execlp(argv[0], argv[0], argv[3], (char *)NULL);
					 perror("fallo en exec C");
					 mq_close(queue1);
					 mq_unlink(argv[2]);
					 mq_close(queue2);
					 mq_unlink(argv[3]);
			     exit(EXIT_FAILURE);
				 }
			 }

		}


	for (i = 0; i < N; i++){
			wait(NULL);
	}

	mq_close(queue1);
	mq_unlink(argv[2]);
	mq_close(queue2);
	mq_unlink(argv[3]);

	return EXIT_SUCCESS;
}

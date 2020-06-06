/**
 * @brief	En este programa, el proceso padre genera dos hijos. Uno de ellos genera un numero aleatorio,
 * lo envia a traves del pipe y lo imprime.
 * El padre lo lee a traves del pipe, y se lo envia al otro hijo utilizando otro pipe. Este hijo lo lee y lo imprime.
 * El programa tiene en cuenta control de errores al crear hijos y pipes, cierre de los respectivos canales del descriptor de ficheros
 * tanto para escribir como para leer y espera del padre por sus hijos.
 * @file p1_exercise_9.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 14-02-2019
 */

 #include <sys/types.h>
 #include <time.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <sys/wait.h>
 #include <unistd.h>

 #define NUM_HIJOS 2

int main(int argc, char *argv[]) {
  int fd[2];
  int pipe_status, i;
  pid_t pid;
  int numero_recibido;

  /** Inicializa el generador con una semilla cualquiera, OJO! este metodo solo
   se llama una vez */
  srand(time(NULL));

   /**creamos la primera tuberia para comunicar al padre con el primer hijo*/
   pipe_status=pipe(fd);
   if(pipe_status == -1) {
   	perror("Error creando la tuberia\n");
   	exit(EXIT_FAILURE);
   }

   pid = fork();
   if(pid <  0){
     printf("Error al emplear fork\n");
     exit(EXIT_FAILURE);
   }

   /**el hijo genera un numero aleatorio y lo envía a través del pipe: escritura*/
  if(pid == 0){
    /** Devuelve un numero aleatorio en 0 y MAX_RAND(un número alto que varia
     segun el sistema) */
    int r = rand();
    /** Cierre del descriptor de lectura en el hijo */
  	close(fd[0]);
  	/** Enviar el numero vía descriptor de salida */
  	write(fd[1], &r, sizeof(int));
    printf("Soy el primer hijo. He generado el numero aleatorio %d\n", r);
  	exit(EXIT_SUCCESS);
  }

  /**el padre lee el numero que le pasa el hijo por pipe*/
  else{
    /** Cierre del descriptor de salida en el padre */
  	close(fd[1]);
  	/* Leer el numero aleatorio de la tubería */
  	read(fd[0], &numero_recibido, sizeof(int));
  	printf("Soy el padre. He leido por la tuberia el numero: %d\n", numero_recibido);

    /**creamos la segunda tuberia para comunicar al padre con el segundo hijo*/
    pipe_status=pipe(fd);
    if(pipe_status == -1) {
    	perror("Error creando la tuberia\n");
    	exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid <  0){
      printf("Error al emplear fork\n");
      exit(EXIT_FAILURE);
    }

    /**el segundo hijo lee lo que le envia el padre por la tuberia*/
   if(pid == 0){
     /** Cierre del descriptor de escritura en el hijo */
     close(fd[1]);
     /** Lee el numero enviado por la tuberia */
     read(fd[0], &numero_recibido, sizeof(int));
     printf("Soy el segundo hijo. He leido el numero %d\n", numero_recibido);
     exit(EXIT_SUCCESS);
   }

   else{
     /** Cierre del descriptor de lectura en el padre */
   	close(fd[0]);
   	/** Enviar el numero vía descriptor de salida */
   	write(fd[1], &numero_recibido, sizeof(int));
    printf("Soy el padre. Voy a enviar el numero al segundo hijo\n");
   }
  }

  for(i = 0; i < NUM_HIJOS; i++){
    wait(NULL);
  }

  exit(EXIT_SUCCESS);
}

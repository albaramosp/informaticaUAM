/**
 * @brief Programa que simula una carrera por ver quien llega antes a mas de 20 puntos, utilizando un fichero para contar las puntuaciones.
 * @file ejercicio9.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 20-03-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define SEM_RECURSO "/escritura_sem"
#define FILENAME "carrera.txt"

#define N_PROC 10

/* Estas variables son globales para que puedan ser utilizadas desde los manejadores, fuera del main*/

/*Un solo semaforo, para controlar el acceso al recurso que queremos leer y escribir*/
sem_t *sem_recurso = NULL;
FILE *f;
int *cuenta = NULL;

/*Al recibir la senyal, el padre espera a sus hijos, cierra el semaforo y termina*/
void manejador_SIGTERM_padre() {
    int i;
    for (i = 0; i < N_PROC; i++){
      wait(NULL);
    }

    sem_close(sem_recurso);
    sem_unlink(SEM_RECURSO);

	  free(cuenta);

    exit(EXIT_SUCCESS);
}

/* Rutina de tratamiento de la señal SIGTERM en los procesos hijos. Cierra el semaforo y termina la ejecucion*/
void manejador_SIGTERM() {
  sem_close(sem_recurso);
  exit(EXIT_SUCCESS);
}

/*Funcion que ejecutan los participantes en la carrera, que escribe el id del proceso en el fichero*/
void escritura (int i){
  f = fopen(FILENAME, "a");
  if (!f){
    perror("Error al abrir el fichero en un hijo");
    exit (EXIT_FAILURE);
  }

  fprintf(f, "%d\n", i);
  fclose(f);
}

/*Funcion que ejecuta el padre. Comprueba si algun participante ha superado los 20 puntos, en cuyo caso cierra el fichero y envia la senyal
* de fin a sus hijos. Si no es el caso, mostrara los resultados actuales y reseteara el fichero
*/
void lectura (){
  char buf[10];
  int id, j;

  f = fopen(FILENAME, "r");
  if (!f){
    perror("Error al abrir el fichero en el padre");
    exit (EXIT_FAILURE);
  }

  while (fgets(buf, sizeof buf, f) != NULL){
    id = atoi(buf);
    cuenta[id]++;
    if (cuenta[id] > 20){
      printf("\n***LA CARRERA HA TERMINADO***\n");
      printf("\n***El ganador es... CHAN CHAN CHAAAAAAAN... ¡¡¡¡¡¡ %d, con %d puntos !!!!!!***\n", id, cuenta[id]);

      fclose(f);
      remove(FILENAME);

      if (kill (0, SIGTERM)){
        perror("\n\nERROR AL ENVIAR A TODO EL GRUPO\n\n");
        exit (EXIT_FAILURE);
      }
    }
  }

  for (j = 0; j < N_PROC; j++){
    printf("Puntos del proceso %d = %d\n", j, cuenta[j]);
  }
  printf("\n");

  fclose(f);
  remove(FILENAME);
}

int main(void) {
  int i;
  pid_t pid;

  srand(time(NULL));
  cuenta = calloc(N_PROC, sizeof(int));

  if ((sem_recurso = sem_open(SEM_RECURSO, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
    perror("sem_open recurso");
    exit(EXIT_FAILURE);
  }


  for (i = 0; i < N_PROC; i++){
    pid = fork();

    if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    //hijo con identificador i
    if (pid == 0) {
      struct sigaction act2;
      act2.sa_handler = manejador_SIGTERM;
      sigemptyset(&(act2.sa_mask));
      act2.sa_flags = SA_RESETHAND;

      if (sigaction(SIGTERM, &act2, NULL) < 0) {
        perror("ERROR CAPTURA SIGTERM");
        exit(EXIT_FAILURE);
      }

      while(1){
        sem_wait(sem_recurso);

        escritura(i);

        sem_post(sem_recurso);

        int secs = rand() % (100000) + 1;
        usleep(secs); //Duerme como mucho 0'1 segundos (decima de segundo)
      }
    }

    //(padre)
    else {
      if(i == (N_PROC-1)) {
        struct sigaction act;
        act.sa_handler = manejador_SIGTERM_padre;
        sigemptyset(&(act.sa_mask));
        act.sa_flags = SA_RESETHAND;

        if (sigaction(SIGTERM, &act, NULL) < 0) {
          perror("ERROR CAPTURA sigterm en el padre");
          exit(EXIT_FAILURE);
        }

        while(1){
            sleep(1);

            sem_wait(sem_recurso);

            lectura();

            sem_post(sem_recurso);
        }
      }
    }
  }
}

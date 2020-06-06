/**
 * @brief Programa que utiliza semaforos para controlar el acceso a recursos protegidos y resolver el problema de los lectores y escritores
 * dando prioridad a los lectores. Demuestra como se intercalan las lecturas y las escrituras para diferentes valores de SECS y N_READ
 * @file ejercicio8.c
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

#define SEM_CONTADOR "/lectura_sem"
#define SEM_RECURSO "/escritura_sem"
#define SEM_LECTORES "/lectores_sem"
#define N_READ 1
#define SECS 0

/* Los semaforos son globales para que puedan ser cerrados desde los manejadores*/

/*Un semaforo para controlar el acceso al contador*/
sem_t *sem_contador = NULL,
/*Un semaforo para controlar el acceso al recurso que queremos leer y escribir*/
	*sem_recurso = NULL,
/*Un semaforo que es el contador en si*/
	*sem_lectores = NULL;

void manejador_SIGINT() {
  printf("He recibido la señal SIGINT \n");

  if (kill (0, SIGTERM)){
    perror("\n\nERROR AL ENVIAR A TODO EL GRUPO\n\n");
    return;
  }
}

void manejador_SIGTERM_padre() {
    int i;
    for (i = 0; i < N_READ; i++){
      wait(NULL);
    }

    sem_close(sem_recurso);
    sem_unlink(SEM_RECURSO);
    sem_close(sem_contador);
    sem_unlink(SEM_CONTADOR);
    sem_close(sem_lectores);
    sem_unlink(SEM_LECTORES);

    exit(EXIT_SUCCESS);
}

void manejador_SIGTERM() {
  sem_close(sem_recurso);
  sem_close(sem_contador);
  sem_close(sem_lectores);

  exit(EXIT_SUCCESS);
}

void escribir(){
  printf("W-INI <%d>\n", getppid());
  sleep(1);
  printf("W-FIN <%d>\n", getppid());
}

void leer(){
  printf("R-INI <%d>\n", getpid());
  sleep(1);
  printf("R-FIN <%d>\n", getpid());
 }

int main(void) {
  int i, aux;
  pid_t pid;

  if ((sem_contador = sem_open(SEM_CONTADOR, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
    perror("sem_open contador");
    exit(EXIT_FAILURE);
  }

  if ((sem_recurso = sem_open(SEM_RECURSO, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
    perror("sem_open recurso");
    exit(EXIT_FAILURE);
  }

  if ((sem_lectores = sem_open(SEM_LECTORES, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
    perror("sem_open lectores");
    exit(EXIT_FAILURE);
  }


  for (i = 0; i < N_READ; i++){
    pid = fork();

    if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    //lector i
    if (pid == 0) {
      printf("Hijo %d creado \n", i);
      struct sigaction act2;
      act2.sa_handler = manejador_SIGTERM;
      sigemptyset(&(act2.sa_mask));
      act2.sa_flags = SA_RESETHAND;

      if (sigaction(SIGTERM, &act2, NULL) < 0) {
        perror("ERROR CAPTURA SIGTERM");
        exit(EXIT_FAILURE);
      }

      while(1){
        sem_getvalue(sem_lectores, &aux);
        printf("*****El contador vale %d*****\n", aux);
        sem_wait(sem_contador);
        sem_post(sem_lectores); //lectores++;
        sem_getvalue(sem_lectores, &aux);
        printf("*****El contador vale %d*****\n", aux);

        if(aux == 1){
          sem_wait(sem_recurso);
        }

        sem_post(sem_contador);

        leer();

        sem_wait(sem_contador);
        sem_wait(sem_lectores);//lectores--;

				/*Volvemos a actualizar el valor de aux, es decir del contador lectores*/
        sem_getvalue(sem_lectores, &aux);
        printf("*****El contador vale %d*****\n", aux);

        if(aux == 0){
          sem_post(sem_recurso);
        }

        sem_post(sem_contador);

        sleep(SECS);
      }
    }

    //escritor (padre)
    else {
      if (i == (N_READ-1)){
        struct sigaction act;
        act.sa_handler = manejador_SIGINT;
        sigemptyset(&(act.sa_mask));
        act.sa_flags = SA_RESETHAND;

        if (sigaction(SIGINT, &act, NULL) < 0) {
          perror("ERROR CAPTURA SIGINT");
          exit(EXIT_FAILURE);
        }

        act.sa_handler = manejador_SIGTERM_padre;
        sigemptyset(&(act.sa_mask));
        act.sa_flags = SA_RESETHAND;

        if (sigaction(SIGTERM, &act, NULL) < 0) {
          perror("ERROR CAPTURA sigterm en el padre");
          exit(EXIT_FAILURE);
        }

        while(1){
          sem_wait(sem_recurso);
          escribir();
          sem_post(sem_recurso);

          sleep(SECS);
        }
      }
    }
  }
}

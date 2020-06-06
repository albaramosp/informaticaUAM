/**
 * @brief Programa que demuestra como una senyal interrumpe la ejecuciono de un bucle infinito cuando llega si esta se encuentra desbloqueada
 * @file ejercicio6b.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 13-03-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define N_ITER 5
#define SECS 40


/* manejador: rutina de tratamiento de la señal SIGTERM. */
void manejador_SIGTERM(int sig) {
   printf("Soy %d y he recibido la señal SIGTERM \n", getpid());
   fflush(stdout);
   printf("\nFin hijo\n");
   exit(EXIT_SUCCESS);
}

int main (void) {
   pid_t pid;
   int counter;

   pid = fork();

   if (pid < 0) {
     perror("fork");
     exit(EXIT_FAILURE);
   }

   if (pid == 0) {
     //preparacion para recibir SIGTERM del padre
     struct sigaction act;
     act.sa_handler = manejador_SIGTERM;
     sigemptyset(&(act.sa_mask));
     act.sa_flags = SA_RESETHAND;

     if (sigaction(SIGTERM, &act, NULL) < 0) {
       perror("SIGTERM en el hijo");
       exit(EXIT_FAILURE);
     }

     while(1) {
       for (counter = 0; counter < N_ITER; counter++) {
           printf("%d\n", counter);
           sleep(1);
         }
         sleep(3);
       }
   }

   if (pid > 0) {
     sleep(SECS);
     //el padre envia SIGTERM al hijo
     if (kill(pid, SIGTERM)){
       perror("Ha habido error en kill. Fin del programa");
       return EXIT_FAILURE;;
     }
   }

   while (wait(NULL) > 0);
   exit(EXIT_SUCCESS);
}

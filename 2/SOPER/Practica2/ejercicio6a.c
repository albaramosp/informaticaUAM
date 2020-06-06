/**
 * @brief Programa que genera una alarma tras 40 segundos de ejecucion y que es bloqueada y desbloqueada en ciertos momentos de la ejecucion.
 * Demuestra como una senyal bloqueada n o puede interrumpir la ejecucion de un proceso.
 * @file ejercicio6a.c
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

int main (void) {
   pid_t pid;
   int counter;

   pid = fork();
   if (pid < 0) {
     perror("fork");
     exit(EXIT_FAILURE);
   }


   if (pid == 0) {
     if (alarm(SECS))
      fprintf(stderr, "Existe una alarma previamente establecida\n");

     while(1) {
       sigset_t set;
       // Máscara que bloqueará las señales SIGUSR1, SIGUSR2 y SIGALRM
       sigemptyset(&set);
       sigaddset(&set, SIGUSR1);
       sigaddset(&set, SIGUSR2);
       sigaddset(&set, SIGALRM);

       //Bloqueo de las señales anteriores
       if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
         perror("sigprocmask block");
         exit(EXIT_FAILURE);
       }

       for (counter = 0; counter < N_ITER; counter++) {
         printf("%d\n", counter);
         sleep(1);
       }

       sigdelset(&set, SIGUSR2);
       
       //Desbloqueo de las señales anteriores menos SIGUSR2
       if (sigprocmask(SIG_UNBLOCK, &set, NULL) < 0) {
         perror("sigprocmask unblock");
         exit(EXIT_FAILURE);
       }

       sleep(3);
     }
   }

   while (wait(NULL) > 0);
}

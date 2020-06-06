/**
 * @brief Programa donde se intenta capturar SIGKILL
 * @file ejercicio3d.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 06-03-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

/* manejador: rutina de tratamiento de la señal SIGKILL. */
void manejador(int sig) {
   printf("He conseguido capturar SIGKILL\n");
   fflush(stdout);
}

int main(void) {
   struct sigaction act;
   act.sa_handler = manejador;
   sigemptyset(&(act.sa_mask));
   act.sa_flags = SA_RESETHAND;

   if (sigaction(SIGKILL, &act, NULL) < 0) {
     perror("sigaction");
     exit(EXIT_FAILURE);
   }

   while(1) {
     printf("En espera de Ctrl+C (PID = %d)\n", getpid());
     sleep(9999);
   }
}

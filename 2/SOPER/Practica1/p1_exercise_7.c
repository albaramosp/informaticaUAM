/**
 * @brief  El programa ejecuta primero el comando ls -l en
 * un proceso hijo, usando la llamada execlp. A continuación, pide al usuario una lista de
 * ficheros que estén en la misma carpeta, separados por comas(','), para luego imprimirlos por pantalla creando
 * un nuevo proceso hijo y usando la llamada execvp para ejecutar el comando cat.
 *
 * @file p1_exercise_7.c
 * @author Alba Ramos: alba.ramosp@estudiante.uam.es; Grupo 2213
 * @author Andrea Salcedo: andrea.salcedo@estudiante.uam.es; Grupo 2213
 * @date 11-02-2019
 */

/** wait and return process info */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
/** library for exec */
#include <unistd.h>
#include<string.h>


/**
 * @author Alba Ramos
 * @author Andrea Salcedo
 * @brief Pide al usuario una serie de ficheros separados por comas, los almacena en
 * un vector de strings, crea un nuevo proceso hijo y en él ejecuta el comando cat con el
 * vector de strings como vector de argumentos
 * @return éxito o fallo de la operación
 */
void  processCat () {
    /** Variables que usa el metodo getline para leer la entrada del usuario */
    char *fileName = NULL;
    size_t fileLen = 0;
    ssize_t fileRead;
    pid_t pid;

    printf("Introduzca los ficheros que quiere mostrar separados por ',' \n");
    while((fileRead = getline(&fileName, &fileLen, stdin)) < 1)
    {
   	 printf("Por favor inserte al menos un fichero \n");
    }

    /** Cuenta el número de ficheros */
    size_t fileCount = 0;
    for(ssize_t i = 0; i < fileRead; i++)
    {
   	 if(fileName[i] == ',' || fileName[i] == '\n')
   	 {
   		 fileCount++;
   	 }
    }

    size_t nArgs = fileCount + 2;
    /** Reserva espacio para argumentos */
    char ** args = malloc(nArgs * sizeof(*args));
    if(args == NULL)
    {
   	 exit(EXIT_FAILURE);
    }

    args[0] = "cat";

    char * filePtr = fileName;
    size_t argIndex = 1;
    for(ssize_t i = 0; i < fileRead; i++)
    {
   	 if(fileName[i] == ',' || fileName[i] == '\n')
   	 {
   		 fileName[i] = '\0';
   		 args[argIndex] = filePtr;
   		 argIndex++;
   		 filePtr = &fileName[i + 1];
   	 }
    }

    args[nArgs - 1] = NULL;

    if (nArgs >  1) {
       /**
     	 * Creamos un nuevo proceso hijo y en el ejecutamos execv para ejecutar el
     	 * comando cat con el vector de argumentos args. El padre debe esperar a que
     	 * el hijo termine
       */
      pid = fork();
   		if(pid <  0){
   			printf("Error al emplear fork\n");
   			exit(EXIT_FAILURE);
   		}

   		else if(pid ==  0){
        execv("/bin/cat", args);
   			exit(EXIT_FAILURE);
   		}

   		else if(pid >  0){
   			wait(NULL);
   		}
    }

    free (args);
    free (fileName);
}

/**
 * @author Alba Ramos
 * @author Andrea Salcedo
 * @brief Pide al usuario una serie de ficheros separados por comas, los almacena en
 * un vector de strings, crea un nuevo proceso hijo y en él ejecuta el comando cat con el
 * vector de strings como vector de argumentos
 * @return éxito o fallo de la operación
 */
void  showAllFiles () {
  pid_t pid;
    /**
    * Creamos un nuevo proceso hijo usando la llamada execlp y en él ejecutamos el
    * comando ls -l. El proceso padre debe de esperar a que el hijo termine.
    */
    pid = fork();
    if(pid <  0){
      printf("Error al emplear fork\n");
      exit(EXIT_FAILURE);
    }

    else if(pid ==  0){
      execlp("ls", "ls", "-l", (char *)NULL);
      exit(EXIT_FAILURE);
    }

    else if(pid >  0){
      wait(NULL);
    }
}

/**
 * @author Alba Ramos
 * @author Andrea Salcedo
 * @brief  La función llama a shoAllFiles para ejecutar el comando ls -l. A continuación, llama a processCat
  *para ejecutar el comando cat.
 * @return éxito o fallo de la operación
 */
int  main(void) {
    showAllFiles();
    processCat();
    exit (EXIT_SUCCESS);
}

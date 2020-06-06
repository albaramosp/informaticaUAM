/**
 * @brief Implements a recursive HTTP server. Supports POST, GET and OPTIONS methods.
 * The server is recursive and uses processes to manage the functionality.
 * Runs as a daemon to avoid having an opened terminal for the main process.
 *
 * @file server.c
 * @author Pair number 15 Redes II Group 2361
 * @version 1.0
 * @date 11-02-2020
 * @copyright GNU Public License
 */

#include "../includes/sockets.h"
#include "../includes/request_response.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int socketDescrServer, socketDescrConn;
HTTPRequest *request;

/**
 * @brief Frees resources and closes sockets when program receives SIGINT signal.
 *        Used to stop the program if not running as a daemon.
 *
 * @author: Pair number 15 Redes II Group 2361
 */
void SIGINT_handler(int sig) {
  printf("\nProcess received SIGTERM\n");
  close(socketDescrConn);
  close(socketDescrServer);
  free(request);
}

int main (){
  struct sigaction act;
  pid_t pid;

  /*Configure handler for SIGINT signal*/
  act.sa_handler = SIGINT_handler;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = SIGINT;

  if (sigaction(SIGINT, &act, NULL) < 0) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  /*Default: the server executes as a daemon*/
  toDaemon();

  /*Creates socket and listens to petitions*/
  socketDescrServer = init_server();

  /*Allocate structure to hold a connection's HTTP request parsed fields*/
  request = (HTTPRequest *)calloc(1, sizeof(HTTPRequest));

  while (1){
    /*Handles each incoming connection in a new socket*/
    socketDescrConn = accept_connection(socketDescrServer);
    if(socketDescrConn == -1){
      continue;
    }

    /*Concurrent server: handles each new connection using a new process*/
    if ( (pid = fork()) == 0 ){
      process_request(socketDescrConn, request);

      free(request);
      request = NULL;

      exit(EXIT_SUCCESS);
    }

    close(socketDescrConn);
  }

  if (request != NULL)
    free(request);

  close(socketDescrServer);

  return EXIT_SUCCESS;

}

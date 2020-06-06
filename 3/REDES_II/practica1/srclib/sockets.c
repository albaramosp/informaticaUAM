/**
 * @brief Implements the functionality related to the use of sockets and connections.
 *
 * @file sockets.c
 * @author Pair number 15 Redes II Group 2361
 * @version 1.0
 * @date 11-02-2020
 * @copyright GNU Public License
 */


#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include "../includes/sockets.h"

#define MAX_STRING 128

int openSocket(int domain, int type, int protocol){
  int socketDescrServer;

  /*Open a socket of the specified domain, type and protocol.*/
  if ( (socketDescrServer = socket(domain, type, protocol)) < 0 ){
    syslog(LOG_ERR, "Error creating socketDescr: %d", errno);
    exit(EXIT_FAILURE);
  }

  return socketDescrServer;
}

void bindSocket(int socket, int domain, int port, int addr){
  struct sockaddr_in dirServer;
  size_t ret;

  dirServer.sin_family=domain;
  dirServer.sin_port=htons(port);
  dirServer.sin_addr.s_addr=htonl(addr);
  bzero((void *)&(dirServer.sin_zero), 8);

  /*Binds socket to address*/
  if ((ret = bind (socket, (struct sockaddr *)&dirServer, sizeof(dirServer))) <0){
    syslog(LOG_ERR, "Error binding socketDescr: %ld", ret);
    exit(EXIT_FAILURE);
  }

  syslog (LOG_INFO, "Binded socketDescr");
}

int init_server(){
  int socketDescrServer, port, max_clients;
  FILE *fp;
  char line[MAX_STRING];

  /*Open the configuration file, server.conf*/
  fp = fopen("server.conf", "r");
  if(fp == NULL){
    syslog(LOG_ERR, "Error opening file server.conf: %d", errno);
    return -1;
  }

  /*Reads the port and max clients from configuration file*/
  while (fgets(line, MAX_STRING, fp)) {
    if (strncmp("listen_port", line, strlen("listen_port")) == 0) {
      strtok(line, " \n");
      strtok(NULL, " \n");
      port = atoi(strtok(NULL, " \n"));
    }

    if (strncmp("max_clients", line, strlen("max_clients")) == 0) {
      strtok(line, " \n");
      strtok(NULL, " \n");
      max_clients = atoi(strtok(NULL, " \n"));
    }
  }

  fclose(fp);

  /*Opens TCP socket*/
  socketDescrServer = openSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  /*Binds socket to address*/
  bindSocket(socketDescrServer, AF_INET, port,  INADDR_ANY);
  syslog (LOG_INFO, "Server started on port %d", port);

  /*Creates the queue to listen for incomming connections*/
  if (listen (socketDescrServer, max_clients) < 0){
    syslog(LOG_ERR, "Error listenining: %d", errno);
    exit(EXIT_FAILURE);
  }

  syslog (LOG_INFO, "Listening connections...");

  return socketDescrServer;
}

int accept_connection(int serverSocket){
  int socketDescrConn;
  socklen_t len;
  struct sockaddr dirConn;

  len = sizeof(dirConn);

  /*Creates new socket for the incoming connection*/
  if ((socketDescrConn = accept(serverSocket, &dirConn, &len))<0){
    syslog(LOG_ERR, "Error accepting connection: %d", errno);
    return -1;
  }

  syslog (LOG_INFO, "Connection accepted");
  return socketDescrConn;
}

void toDaemon(){
  pid_t pid;
  char currentDir[MAX_STRING];

  /*Stores current working directory to come back to it afterwards*/
  getcwd(currentDir, sizeof(currentDir));
  syslog(LOG_INFO, "Current dir is %s", currentDir);

  pid = fork();
  if (pid < 0) exit(EXIT_FAILURE);
  if (pid > 0) {
    exit(EXIT_SUCCESS); /*Kill parent process*/
  }

  umask(0);
  setlogmask (LOG_UPTO (LOG_INFO));
  openlog ("Server system messages:", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL3);
  syslog (LOG_INFO, "Initiating new server...");

  /*Child process is adopted by main process*/
  if (setsid()< 0) {
    syslog (LOG_ERR, "Error creating a new SID for the child process.");
    exit(EXIT_FAILURE);
  }

  /*Changes the current directory to the one of the parent*/
  if ((chdir("/")) < 0) {
    syslog (LOG_ERR, "Error changing the current working directory = \"/\"");
    exit(EXIT_FAILURE);
  }

  /*Close associated file descriptors*/
  syslog(LOG_INFO, "Closing standard file descriptors...");
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  chdir(currentDir); /*Return to the previous working directory*/

  syslog (LOG_INFO, "Daemonized correctly");

  return;
}

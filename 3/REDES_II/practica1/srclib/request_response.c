
/**
 * @brief Implements the functionality associated with the requests made to the server.
 *
 * @file request_response.c
 * @author Pair number 15 Redes II Group 2361
 * @version 1.0
 * @date 11-02-2020
 * @copyright GNU Public License
 */

#include "../includes/request_response.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_BUFFER 1024
#define MAX_STRING 128
#define NUM_PARAMETERS 3

/** @struct mime_map
 *
 *  @brief Holds the extension associated to a mime_type
 */
typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;


mime_map extensions_types [] = {
    {".txt", "text/plain"},
    {".html", "text/html"},
    {".htm", "text/html"},
    {".gif", "image/gif"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".doc", "application/msword"},
    {".docx", "application/msword"},
    {".pdf", "application/pdf"},
    {".py", "text/html"},
    {".php", "text/html"},
    {NULL, NULL},
};

int process_request(int connDescr, HTTPRequest *request){
  char* body;
  char buf[MAX_BUFFER], method[MAX_STRING], path[MAX_STRING], aux[MAX_BUFFER];
  char line[MAX_STRING], route[MAX_STRING], name[MAX_STRING];
  int pret;
  size_t buflen = 0, prevbuflen = 0;
  ssize_t rret;
  FILE *fp;

  while (1) {
    /*Read the request*/
    while ((rret = read(connDescr, buf + buflen, MAX_BUFFER - buflen)) == -1 && errno == EINTR);

    if (rret <= 0)
        return -1;

    prevbuflen = buflen;
    buflen += rret;

    /*Parse the request using PicoHTTPParser*/
    request->num_headers = sizeof(request->headers) / sizeof(request->headers[0]);
    pret = phr_parse_request(buf, buflen, &(request->method), &(request->method_len), &(request->path), &(request->path_len),
                              &(request->minor_version), request->headers, &(request->num_headers), prevbuflen);

    if (pret > 0)
        break; /*Successfully parsed the request*/
    else if (pret == -1)
        return -1;

    /*Request is incomplete, continue the loop*/
    assert(pret == -2);

    if (buflen == sizeof(buf)){
        syslog(LOG_ERR,"RequestIsTooLongError\n");
        return -1;
    }
  }

  /*Store request method*/
  sprintf(method, "%.*s", (int)request->method_len, request->method);

  /*Store request body*/
  sprintf(aux, "%.*s", (int)buflen, buf);
  body = strrchr(aux, '\n')+1;

  /*Open the server configuration file, server.conf*/
  fp = fopen("server.conf", "r");

  if(fp == NULL){
    syslog(LOG_ERR, "Error opening file server.conf: %d", errno);
    return -1;
  }

  /*Reads the server_signature and server_route from server.conf*/
  while (fgets(line, MAX_STRING, fp)) {
    if (strncmp("server_route", line, strlen("server_route")) == 0) {
      strtok(line, " \n");
      strtok(NULL, " \n");
      sprintf(route, "%s", strtok(NULL, " \n"));
    } if (strncmp("server_signature", line, strlen("server_signature")) == 0) {
      strtok(line, " \n");
      strtok(NULL, " \n");
      sprintf(name, "%s", strtok(NULL, " \n"));
    }
  }

  fclose(fp);

  /*Write the route to the resource -path-*/
  if (request->path_len > 1) {
    sprintf(path, "%s%.*s", route, (int)request->path_len, request->path);
  } else {
    sprintf(path, "%s/index.html", route);
  }

  /*Execute functionality according to method*/
  if (strcmp(method, "GET") == 0){
    process_GET(connDescr, request, name, path);
  } else if (strcmp(method, "POST") == 0){
    process_POST(connDescr, request, name, path, body);
  } else if (strcmp(method, "OPTIONS") == 0){
    process_OPTIONS(connDescr, request, name);
  } else {
    process_unsupported(connDescr, request, name);
  }

  return pret;
}

int process_POST(int connDescr, HTTPRequest *request, char* serverSignature, char* path, char* request_body){
  char aux[MAX_STRING];
  char response[MAX_BUFFER], body[MAX_BUFFER], script_response[MAX_BUFFER];
  int lenght, nParams;
  char *dot, *params, *token, *save1, *save2;
  time_t t = time(NULL);
  struct stat stats;
  mime_map *map = extensions_types;
  Parameter parameters[NUM_PARAMETERS];

  /*Parse the parameters in the URL*/
  nParams = 0;
  if (strrchr(path, '?') != NULL)
    params = strrchr(path, '?')+1;
  else
    params = NULL;

  if (params != NULL){
    path[strrchr(path, '?')-path] = '\0'; /*Remove params from real path*/

    /*There is only one parameter*/
    if (strchr(params, '&') == NULL){
      token = strtok_r(params, "&", &save1);
      parameters[nParams].value = strchr(token, '=')+1;
      parameters[nParams].key = strtok(token, "=");
      nParams++;
    } else {
      token = strtok_r(params, "&", &save1);

      while(token != NULL){
        parameters[nParams].value = strchr(token, '=')+1;
        parameters[nParams].key = strtok_r(token, "=", &save2);
        nParams++;

        token = strtok_r(NULL, "&\0", &save1);
      }
    }
  }

  /*Remove '?' from URL, if present*/
  if (strrchr(path, '?') != NULL){
    path[strrchr(path, '?')-path] = '\0'; /*Remove params from real path*/
  }

  /*Parse the arguments in the body*/
  params = request_body;

  /*There is only one parameter*/
  if (strchr(params, '&') == NULL){
    token = strtok_r(params, "&", &save1);
    parameters[nParams].value = strchr(token, '=')+1;
    parameters[nParams].key = strtok(token, "=");
    nParams++;
  } else {
    token = strtok_r(params, "&", &save1);

    while(token != NULL){
     parameters[nParams].value = strchr(token, '=')+1;
      parameters[nParams].key = strtok_r(token, "=", &save2);
      nParams++;

      token = strtok_r(NULL, "&\0", &save1);
    }
  }

  /*Check if resource exists*/
  if(stat(path, &stats) == 0){
    /*Stores the extension of the file*/
    dot = strrchr(path, '.');

    /*Map the extension with the corresponding mime-type*/
    while(map->extension){
        if(strcmp(map->extension, dot) == 0){
            sprintf(aux, "%s", map->mime_type);
            break;
        }
        map++;
    }

    /*Build response with headers*/
    sprintf(response, "HTTP/1.%d 200 OK\r\n", request->minor_version);
    send(connDescr, response, strlen(response), 0);
    sprintf(response, "Date: %.*s\r\n", (int) strlen(ctime(&t))-1, ctime(&t));
    send(connDescr, response, strlen(response), 0);
    sprintf(response, "Server: %s\r\n", serverSignature);
    send(connDescr, response, strlen(response), 0);
    sprintf(response, "Last-Modified: %.*s\r\n", (int) strlen(ctime(&stats.st_mtime))-1, ctime(&stats.st_mtime));
    send(connDescr, response, strlen(response), 0);

    /*Check if resource is a script, in order to execute it*/
    if((strcmp(dot, ".py") == 0) || (strcmp(dot, ".php") == 0)){
      create_script_response(path, dot, parameters, nParams, script_response);

      lenght = sprintf(body,
          "<doctype html>\n"
          "<html>\n"
          "<head><meta charset=\"UTF-8\"></head>\n"
          "<body><pre>%s</pre></body>\n"
          "<html>", script_response);

      sprintf(response, "Content-Length: %d\r\n", lenght);
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "Content-Type: text/html\r\n");
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "\r\n");
      send(connDescr, response, strlen(response), 0);

      send(connDescr, body, strlen(body), 0);

    } else {
      sprintf(response, "Content-Length: %d\r\n", (int) stats.st_size);
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "Content-Type: %s\r\n", aux);
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "\r\n");
      send(connDescr, response, strlen(response), 0);

      /*Send resource*/
      send_file(connDescr, path);
    }
  } else {
    process_404(connDescr, request, serverSignature);
  }
  return 0;
}

int process_GET(int connDescr, HTTPRequest *request, char* serverSignature, char* path){
  char aux[MAX_STRING];
  char response[MAX_BUFFER], body[MAX_BUFFER], script_response[MAX_BUFFER];
  int lenght, nParams;
  char *dot, *params, *token, *save1, *save2;
  time_t t = time(NULL);
  struct stat stats;
  mime_map *map = extensions_types;
  Parameter parameters[NUM_PARAMETERS];

  /*Parse the parameters in the URL*/
  if (strrchr(path, '?') != NULL)
    params = strrchr(path, '?')+1;
  else
    params = NULL;

  if (params != NULL){
    path[strrchr(path, '?')-path] = '\0'; /*Remove params from real path*/
    nParams = 0;

    /*There is only one parameter*/
    if (strchr(params, '&') == NULL){
      token = strtok_r(params, "&", &save1);
      parameters[nParams].value = strchr(token, '=')+1;
      parameters[nParams].key = strtok(token, "=");
      nParams++;
    } else {
      token = strtok_r(params, "&", &save1);

      while(token != NULL){
        parameters[nParams].value = strchr(token, '=')+1;
        parameters[nParams].key = strtok_r(token, "=", &save2);
        nParams++;

        token = strtok_r(NULL, "&\0", &save1);
      }
    }
  }

  /*Check if resource exists*/
  if(stat(path, &stats) == 0){
    /*Stores the extension of the file*/
    dot = strrchr(path, '.');

    /*Map the extension with the corresponding mime-type*/
    while(map->extension){
        if(strcmp(map->extension, dot) == 0){
            sprintf(aux, "%s", map->mime_type);
            break;
        }
        map++;
    }

    /*Build response with headers*/
    sprintf(response, "HTTP/1.%d 200 OK\r\n", request->minor_version);
    send(connDescr, response, strlen(response), 0);
    sprintf(response, "Date: %.*s\r\n", (int) strlen(ctime(&t))-1, ctime(&t));
    send(connDescr, response, strlen(response), 0);
    sprintf(response, "Server: %s\r\n", serverSignature);
    send(connDescr, response, strlen(response), 0);
    sprintf(response, "Last-Modified: %.*s\r\n", (int) strlen(ctime(&stats.st_mtime))-1, ctime(&stats.st_mtime));
    send(connDescr, response, strlen(response), 0);

    /*If resource is a script, execute it*/
    if((strcmp(dot, ".py") == 0) || (strcmp(dot, ".php") == 0)){
      create_script_response(path, dot, parameters, nParams, script_response);

      lenght = sprintf(body,
          "<doctype html>\n"
          "<html>\n"
          "<head><meta charset=\"UTF-8\"></head>\n"
          "<body><pre>%s</pre></body>\n"
          "<html>", script_response);

      sprintf(response, "Content-Length: %d\r\n", lenght);
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "Content-Type: text/html\r\n");
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "\r\n");
      send(connDescr, response, strlen(response), 0);

      send(connDescr, body, strlen(body), 0);

    } else {
      sprintf(response, "Content-Length: %d\r\n", (int) stats.st_size);
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "Content-Type: %s\r\n", aux);
      send(connDescr, response, strlen(response), 0);
      sprintf(response, "\r\n");
      send(connDescr, response, strlen(response), 0);

      /*Send requested resource*/
      send_file(connDescr, path);
    }
  } else {
    process_404(connDescr, request, serverSignature);
  }
  return 0;
}

int process_OPTIONS(int connDescr, HTTPRequest *request, char* serverSignature){
  char response[MAX_BUFFER];
  time_t t = time(NULL);

  /*curl -i -X OPTIONS http://localhost:1025*/
  /*Use -i option to see the response's header*/
  sprintf(response, "HTTP/1.%d 204 No Content\r\n", request->minor_version);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Allow: OPTIONS, GET, POST\r\n");
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Date: %.*s\r\n", (int) strlen(ctime(&t))-1, ctime(&t));
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Server: %s\r\n", serverSignature);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Content-Length: 0\r\n");
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Content-Type: text/html\r\n");
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "\r\n");
  send(connDescr, response, strlen(response), 0);

  return 0;
}

int process_unsupported(int connDescr, HTTPRequest *request, char* serverSignature){
  char response[MAX_BUFFER];
  char body[MAX_BUFFER];
  int lenght;
  time_t t = time(NULL);

  /*Get response's length*/
  lenght = sprintf(body,
          "<doctype html>\n"
          "<html>\n"
          "<head><meta charset=\"UTF-8\"></head>\n"
          "<body><h1>Error 418</h1><p>I'm a teapot, not a coffee pot!</p></body>\n"
          "<html>");


  sprintf(response, "HTTP/1.%d 418 I'm a teapot, not a coffee pot!\r\n", request->minor_version);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Date: %.*s\r\n", (int) strlen(ctime(&t))-1, ctime(&t));
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Server: %s\r\n", serverSignature);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Content-Length: %d\r\n", lenght);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Content-Type: text/html\r\n");
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "\r\n");
  send(connDescr, response, strlen(response), 0);

  send(connDescr, body, strlen(body), 0);

  return 0;
}

int process_404(int connDescr, HTTPRequest *request, char* serverSignature){
  char response[MAX_BUFFER];
  char body[MAX_BUFFER];
  int lenght;
  time_t t = time(NULL);

  /*Get response's length*/
  lenght = sprintf(body,
          "<doctype html>\n"
          "<html>\n"
          "<head><meta charset=\"UTF-8\"></head>\n"
          "<body><h1>Error 404</h1><p>%.*s not found</p></body>\n"
          "<html>", (int)request->path_len, request->path);

  sprintf(response, "HTTP/1.%d 404 Not found\r\n", request->minor_version);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Date: %.*s\r\n", (int) strlen(ctime(&t))-1, ctime(&t));
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Server: %s\r\n", serverSignature);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Content-Length: %d\r\n", lenght);
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "Content-Type: text/html\r\n");
  send(connDescr, response, strlen(response), 0);
  sprintf(response, "\r\n");
  send(connDescr, response, strlen(response), 0);

  send(connDescr, body, strlen(body), 0);

  return 0;
}

int create_script_response(char* path, char* extension, Parameter* parameters, int nParams, char* script_response){
  int len;
  char buf[MAX_BUFFER], exe[MAX_BUFFER], command[MAX_STRING];
  FILE *script;

  if(!path || !extension || !parameters || nParams < 0 || !script_response){
    syslog(LOG_ERR, "Error with the arguments in function send_file");
    return -1;
  }

  /*Store the command to execute the script depending on its extension -python or php-*/
  if(strcmp(extension, ".py") == 0){
    sprintf(command, "python3");
  } else if (strcmp(extension, ".php") == 0){
    sprintf(command, "php");
  }

  /*Pass the parameters*/
  switch(nParams){
    case 0:
      sprintf(exe, "%s %s", command, path);
      break;
    case 1:
      sprintf(exe, "%s %s %s", command, path, parameters[0].value);
      break;
    case 2:
      sprintf(exe, "%s %s %s %s", command, path, parameters[0].value, parameters[1].value);
      break;
    case 3:
      sprintf(exe, "%s %s %s %s %s", command, path, parameters[0].value, parameters[1].value, parameters[2].value);
      break;
    default:
      syslog(LOG_ERR, "Error to many script arguments");
      return -1;
  }

  /*Execute script with popen*/
  script = popen(exe, "r");
  if(!script){
    syslog(LOG_ERR, "Error executing script");
    return -1;
  }

  strcpy(script_response, "");

  while(!feof(script)){
    len = fread(buf, sizeof(char), MAX_BUFFER, script);

    if(len < 0){
      syslog(LOG_ERR, "Error while reading the file");
      pclose(script);
      return -1;
    } else if (len > 0){
      /*Store script response*/
      strncat(script_response, buf, len);
      strcat(script_response, "\n");
    }
  }

  pclose(script);

  return 0;
}

int send_file(int connDescr, char* path){
  int len;
  char buf[MAX_BUFFER];
  FILE *fp = NULL;

  if(connDescr < 0 || !path){
    syslog(LOG_ERR, "Error with the arguments in function send_file");
    return -1;
  }

  /*Open resource*/
  fp = fopen(path, "rb");
  if(!fp){
    syslog(LOG_ERR, "Error oppening the file errno: %d", errno);
    return -1;
  }

  while(!feof(fp)){
    len = fread(buf, sizeof(char), MAX_BUFFER, fp);

    if(len < 0){
      syslog(LOG_ERR, "Error while reading the file");

      fclose(fp);

      return -1;
    } else if (len > 0){
      /*Send resource in parts of MAX_BUFFER length*/
      send(connDescr, buf, len, 0);
    }
  }

  fclose(fp);
  return 0;
}

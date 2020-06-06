#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#define LENGTH 50

int main(int argc, char const *argv[]) {
  char command[LENGTH];
  char query[512];

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */

    if(argc < 2){
      printf("\nError: introducir al menos el nombre del programa y un argumento\n");
      return -1;
    }

    strcpy(command, argv[1]);

    if(strcmp(command, "new") == 0){

      if(argc != 4){
        printf("Error en los argumentos: adjuntar los screenname de los usuarios seguidor y seguido");
        return -1;
      }

      /* CONNECT */
      ret = odbc_connect(&env, &dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }

      /* Allocate a statement handle */
      SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

      /*INSERTAR UNA RELACION SEGUIDOR-SEGUIDO*/
      sprintf(query, "insert into follows select u1.user_id, u2.user_id from users as u1, users as u2 where u1.screenname = '%s' and u2.screenname='%s'", argv[2], argv[3]);
      /*printf("\nQuery: %s\n", query);*/

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Se ha introducido la relacion de seguidor-seguido\n");

      SQLCloseCursor(stmt);


      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }


    }

    if(strcmp(command, "remove") == 0){

      if(argc != 4){
        printf("Error en los argumentos: adjuntar los screenname de los usuarios seguidor y seguido");
        return -1;
      }

      /* CONNECT */
      ret = odbc_connect(&env, &dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }

      /* Allocate a statement handle */
      SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

      /*ELIMINAR UNA RELACION SEGUIDOR-SEGUIDO*/
      sprintf(query, "delete from follows where userfollower = (select user_id from users where screenname='%s') and userfolloweed = (select user_id from users where screenname='%s')", argv[2], argv[3]);
      /*printf("\nQuery: %s\n", query);*/

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Se ha eliminado la relacion de seguidor-seguido\n");

      SQLCloseCursor(stmt);


      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }


    }

    return 0;
}

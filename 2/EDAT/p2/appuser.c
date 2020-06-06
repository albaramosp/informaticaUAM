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

  SQLBIGINT user_id;

    if(argc < 2){
      printf("\nError: introducir al menos el nombre del programa y un argumento\n");
      return -1;
    }

    strcpy(command, argv[1]);

    if(strcmp(command, "new") == 0){

      if(argc != 4){
        printf("Error en los argumentos: adjuntar screenname y localizacion del usuario");
        return -1;
      }

      /* CONNECT */
      ret = odbc_connect(&env, &dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }

      /* Allocate a statement handle */
      SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

      /*INSERTAR UN USUARIO NUEVO, DEVOLVIENDO SU ID ASIGNADO*/

      /*printf("\nQuery: %s\n", query);*/

      sprintf(query, "insert into users (screenname, location, usercreated) values ('%s', '%s', localtimestamp) on conflict (screenname) do nothing returning user_id", argv[2], argv[3]);
      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      SQLBindCol(stmt, 1, SQL_C_SBIGINT, &user_id, sizeof(SQLBIGINT), NULL);

      printf("\n Id del usuario = ");

      /* Loop through the rows in the result-set */
      while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
          printf("%ld", user_id);
      }

      printf("\n");

      SQLCloseCursor(stmt);


      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }


    }

    if(strcmp(command, "remove") == 0){
      if(argc != 3){
        printf("Error en los argumentos: adjuntar solo screenname del usuario a eliminar");
        return -1;
      }

      /* CONNECT */
      ret = odbc_connect(&env, &dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }

      /* Allocate a statement handle */
      SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

      /*ELIMINAR UN USUARIO*/
		
	  /*MODIFICAR LA FK RETWEETS PARA BORRAR EN CASCADA*/
	  sprintf(query, "alter table tweets drop constraint tweets_retweet_fkey");
      printf("\nQuery: %s\n", query);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Se ha eliminado la FK retweets\n");

      SQLCloseCursor(stmt);
		
	  sprintf(query, "alter table tweets add constraint tweets_retweet_fkey foreign key (retweet) references tweets (tweet_id) on delete cascade");
      printf("\nQuery: %s\n", query);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Se ha modificado la FK retweets agregando on delete cascade\n");

      SQLCloseCursor(stmt);

      /*ELIMINAR SUS TWEETS*/
      sprintf(query, "delete from tweets where userwriter in (select user_id from users where screenname='%s')", argv[2]);
      printf("\nQuery: %s\n", query);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Eliminados los tweets del usuario de la tabla tweets\n");

      SQLCloseCursor(stmt);

      /*ELIMINAR SUS SEGUIDORES*/
      sprintf(query, "delete from follows where userfolloweed in (select user_id from users where screenname='%s')", argv[2]);
      printf("\nQuery: %s\n", query);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Eliminados los seguidores del usuario de la tabla follows\n");

      SQLCloseCursor(stmt);

      /*ELIMINAR SUS SEGUIDOS*/
      sprintf(query, "delete from follows where userfollower in (select user_id from users where screenname='%s')", argv[2]);
      printf("\nQuery: %s\n", query);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Eliminados los seguidos del usuario de la tabla follows\n");

      SQLCloseCursor(stmt);

      /*FINALMENTE ELIMINAR EL USUARIO DE LA TABLA USERS*/
      sprintf(query, "delete from users where screenname='%s'", argv[2]);
      printf("\nQuery: %s\n", query);

      SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

      printf("\n Eliminado el usuario de la tabla users\n");

      SQLCloseCursor(stmt);


      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }


    }

    return 0;
}

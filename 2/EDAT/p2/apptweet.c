#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#define LENGTH 255

int main(int argc, char const *argv[]) {
    char command[LENGTH];
    char tweettext[LENGTH] = "\0";
    char query[512];
    int i;

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
        if(argc < 3){
          printf("Error en los argumentos: adjuntar solo el id del tweet a eliminar");
          return -1;
        }

        /*Concatenacion de los argumentos para tener el tweettext entero en una sola variable*/
        for(i = 3;i < argc;i++){
            strcat(tweettext,argv[i]);
            strcat(tweettext," ");
        }

        /* CONNECT */
        ret = odbc_connect(&env, &dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }

        /* Allocate a statement handle */
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

        /*CREAR UN TWEET*/
        sprintf(query, "insert into tweets(tweettext, tweettimestamp, retweet, userwriter) values('%s', localtimestamp,null, (select user_id from users where screenname = '%s'))", tweettext, argv[2]);
        /*printf("\nQuery: %s\n", query);*/

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        printf("\nTweet insertado\n");

        SQLCloseCursor(stmt);

        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }

    }



    if(strcmp(command, "remove") == 0){
        if(argc != 3){
          printf("Error en los argumentos: adjuntar solo el id del tweet a eliminar");
          return -1;
        }

        /* CONNECT */
        ret = odbc_connect(&env, &dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }

        /* Allocate a statement handle */
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);


        /*ELIMINAR LOS RETWEETS*/
        sprintf(query, "delete from tweets where retweet = %s", argv[2]);
        /*printf("\nQuery: %s\n", query);*/

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        printf("\n Reweets eliminados\n");

        SQLCloseCursor(stmt);


        /*ELIMINAR UN  TWEET*/
        sprintf(query, "delete from tweets where tweet_id = %s", argv[2]);
        /*printf("\nQuery: %s\n", query);*/

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        printf("\n Tweet eliminado\n");

        SQLCloseCursor(stmt);

        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }

    }

    if(strcmp(command, "retweet") == 0){
        if(argc != 4){
          printf("Error en los argumentos: adjuntar screenname y id del tweet");
          return -1;
        }

        /* CONNECT */
        ret = odbc_connect(&env, &dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }

        /* Allocate a statement handle */
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

        /*INSERTAR UN RETWEET NUEVO, DEVOLVIENDO SU ID ASIGNADO*/
        sprintf(query, "insert into tweets(tweettext, tweettimestamp, retweet, userwriter) values((select tweettext	from tweets where tweet_id = %s), localtimestamp,%s, (select user_id from users where screenname = '%s'))",
         argv[3], argv[3], argv[2]);
        /*printf("\nQuery: %s\n", query);*/

        SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

        printf("\nRetweet insertado\n");

        SQLCloseCursor(stmt);

        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }


    }





    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>

#include "table.h"
#include "type.h"
#include "odbc.h"

#define NCOLS 4
#define MAX 512

int main(int argc, char const *argv[]) {
  table_t *tb = NULL;
  char query[MAX];
  char comment[MAX] = "\0";
  int i;
  void **buff = NULL;
  int pos, r=0;

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt, stmt2;
  SQLRETURN ret; /* ODBC API return status */

  SQLINTEGER tweets_num;
  SQLCHAR tweet_text[MAX];

  if(argc != 2){
    printf("\nError: introducir la puntuacion\n");
    return -1;
  }

  tb = table_open("score.txt");

  pos = table_first_pos(tb);

  buff = (void **) malloc(NCOLS * sizeof(void *));
  if(buff==NULL) return EXIT_FAILURE;

  buff[0] = (long int *) malloc(sizeof(long int));
  if(buff[0]==NULL) return EXIT_FAILURE;
  buff[1] = (char *) malloc(MAX * sizeof(char));
  if(buff[1]==NULL) return EXIT_FAILURE;
  buff[2] = (int *) malloc(sizeof(int));
  if(buff[2]==NULL) return EXIT_FAILURE;
  buff[3] = (char *) malloc(MAX * sizeof(char));
  if(buff[3]==NULL) return EXIT_FAILURE;

  while(1){
      pos = table_read_record(tb, pos);
      if(pos == -1){
          break;
      }
      r++;

      for(i=0; i < table_ncols(tb); i++) {
          buff[i] = table_column_get(tb, i);
      }

      if(*((int *)buff[2]) > atoi(argv[1]) ){
        /*  printf("[Table Row: %d]\tCol 1: %lld\tCol2: %s\tCol3: %d\tCol4: %s\n",
          r, *((long long *)buff[0]), (char *)buff[1], *((int *)buff[2]), (char *)buff[3]);
          */
          printf("\nScreenname: %s\tPuntuacion: %d\nComentario: %s\n",
            (char *)buff[1], *((int *)buff[2]), (char *)buff[3]);



            /* CONNECT */
            ret = odbc_connect(&env, &dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }

            /* Allocate a statement handle */
            SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

            /*NUMERO DE TWEETS*/
            sprintf(query, "select count(*) from tweets where userwriter in ( select user_id from users where screenname='%s')", (char *)buff[1]);
            /*printf("\nQuery: %s\n", query);*/

            SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

            SQLBindCol(stmt, 1, SQL_C_SLONG, &tweets_num, sizeof(SQLINTEGER), NULL);

            printf("\n Numero de tweets = ");

            /* Loop through the rows in the result-set */
            while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
                printf("%d\n", tweets_num);
            }

            SQLCloseCursor(stmt);

            /*LISTA DE TWEETS*/
            sprintf(query, "select tweet_id, tweettimestamp, tweettext from tweets, users where userwriter=user_id and screenname='%s' order by tweettimestamp asc", (char *)buff[1]);

            /*printf("\nQuery: %s\n", query);*/

            SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

            SQLBindCol(stmt, 3, SQL_C_CHAR, tweet_text, sizeof(tweet_text), NULL);

            printf("\n Lista de tweets:\n");

            /* Loop through the rows in the result-set */
            while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
                printf("%s\n", tweet_text);
            }

            SQLCloseCursor(stmt);

            /* DISCONNECT */
            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }
      }

  }
  
  table_close(tb);


  return 0;
}

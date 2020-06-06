#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>

#include "table.h"
#include "index.h"
#include "type.h"
#include "odbc.h"

#define NCOLS 4
#define MAX 512

int main(int argc, char const *argv[]) {
  index_t *index = NULL;
  table_t *tb = NULL;
  char query[MAX];
  int i, j, k;
  void **buff = NULL;
  int n_times;
  long **directions = NULL;

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */

  SQLINTEGER tweets_num;
  SQLCHAR tweet_text[MAX];

  if(argc != 2){
    printf("\nError: introducir la puntuacion\n");
    return -1;
  }

  index = index_open("index.txt");
  if(!index){
	  return -1;
  }

  buff = (void **) malloc(NCOLS * sizeof(void *));
  if(!buff){
	  index_close(index);
	  return -1;
  }

  buff[0] = (long int *) malloc(sizeof(long int));
  if(!buff[0]){
	  free(buff);
	  index_close(index);
	  return -1;
  }

  buff[1] = (char *) malloc(MAX * sizeof(char));
  if(!buff[1]){
	  free(buff[0]);
	  free(buff);
	  index_close(index);
	  return -1;
  }

  buff[2] = (int *) malloc(sizeof(int));
  if(!buff[2]){
	  free(buff[0]);
	  free(buff[1]);
	  free(buff);
	  index_close(index);
	  return -1;
  }

  buff[3] = (char *) malloc(MAX * sizeof(char));
  if(!buff[3]){
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff);
	  index_close(index);
	  return -1;
  }


  /*check existance in index for each value greater than the argument*/
  for(i = atoi(argv[1])+1; i <= 100; i++){
	  directions = index_get(index, i, &n_times);
	  /*show info if this key appears at least one time*/
	  if(n_times > 0){
		  /*printf("\nVeces clave: %d\n", n_times);
		  printf("\nRegistros direcciones: ");
      for(j = 0;j < n_times;j++){
        printf("%ld ", *(directions[j]));
      }
      printf("\n");*/

		  tb = table_open("score.txt");

		  /*for each direction of that score, give its info*/
		  for(j = 0; j < n_times; j++){
		  	table_read_record(tb, *(directions[j]));
  			for(k=0; k < table_ncols(tb); k++) {
  		    	buff[k] = table_column_get(tb, k);
  			}

			printf("\nScreenname: %s\tPuntuacion: %d\nComentario: %s\n",
            (char *)buff[1], *((int *)buff[2]), (char *)buff[3]);

			ret = odbc_connect(&env, &dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }

            SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

            sprintf(query, "select count(*) from tweets where userwriter in ( select user_id from users where screenname='%s')", (char *)buff[1]);

            SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

            SQLBindCol(stmt, 1, SQL_C_SLONG, &tweets_num, sizeof(SQLINTEGER), NULL);

            printf("\n Numero de tweets = ");

            while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
                printf("%d\n", tweets_num);
            }

            SQLCloseCursor(stmt);

            sprintf(query, "select tweet_id, tweettimestamp, tweettext from tweets, users where userwriter=user_id and screenname='%s' order by tweettimestamp asc", (char *)buff[1]);


            SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

            SQLBindCol(stmt, 3, SQL_C_CHAR, tweet_text, sizeof(tweet_text), NULL);

            printf("\n Lista de tweets:\n");

            while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
                printf("%s\n", tweet_text);
            }

            SQLCloseCursor(stmt);

            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
      				table_close(tb);
      				free(buff[0]);
      				free(buff[1]);
      				free(buff[2]);
      				free(buff[3]);
      				free(buff);
      				return EXIT_FAILURE;
            }
		  }
	  }
  }


  free(directions);
  table_close(tb);
  index_close(index);

  return 0;
}

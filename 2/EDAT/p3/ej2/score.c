#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <unistd.h>

#include "table.h"
#include "type.h"
#include "odbc.h"

#define NCOLS 4
#define MAX 512

int main(int argc, char const *argv[]) {
  type_t types[NCOLS] = {LLNG, STR, INT, STR};
  table_t *tb = NULL;
  char query[MAX];
  char comment[MAX] = "\0";
  int i;
  void **buff = NULL;
  /*int pos, r=0;*/

  SQLBIGINT user_id;
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */

  if(argc < 3){
    printf("\nError: introducir el nombre de usuario, puntuación y comentario\n");
    return -1;
  }

  for(i = 3;i < argc;i++){
      strcat(comment,argv[i]);
      strcat(comment," ");
  }

  /* CONNECT */
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  /* Allocate a statement handle */
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*COGER EL ID DEL USUARIO*/
  sprintf(query, "select user_id from users where screenname = '%s'", argv[1]);
  /*printf("\nQuery: %s\n", query);*/

  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_SBIGINT, &user_id, sizeof(SQLBIGINT), NULL);
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      /*printf("%ld", user_id);*/
  }

  SQLCloseCursor(stmt);

  if( access("score.txt", F_OK ) != -1 ) {
  } else {
    table_create("score.txt", NCOLS, types);
  }


  tb = table_open("score.txt");

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

  /*printf("\n%s, %d, %s\n", argv[1], atoi(argv[2]), argv[3]);*/

  *((long long *) buff[0]) = user_id;
  strcpy((char *) buff[1], argv[1]);
  *((int *) buff[2]) = atoi(argv[2]);
  strcpy((char *) buff[3], comment);


  table_insert_record(tb, buff);

  free(buff[0]);
  free(buff[1]);
  free(buff[2]);
  free(buff[3]);

/*
  pos = table_first_pos(tb);

  while(1){
      pos = table_read_record(tb, pos);
      if(pos == -1){
          break;
      }
      r++;

      for(i=0; i < table_ncols(tb); i++) {
          buff[i] = table_column_get(tb, i);
      }

      printf("[Table Row: %d]\tCol 1: %lld\tCol2: %s\tCol3: %d\tCol4: %s\n",
      r, *((long long *)buff[0]), (char *)buff[1], *((int *)buff[2]), (char *)buff[3]);
  }

*/

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  table_close(tb);

  free(buff);


  return 0;
}

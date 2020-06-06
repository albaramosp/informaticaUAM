#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <unistd.h>

#include "table.h"
#include "type.h"
#include "odbc.h"
#include "index.h"

#define NCOLS 4
#define MAX 512

int main(int argc, char const *argv[]) {
  type_t types[NCOLS] = {LLNG, STR, INT, STR};
  table_t *tb = NULL;
  index_t *index = NULL;
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

  if( access("index.txt", F_OK ) != -1 ) {
  } else {
    index_create(INT, "index.txt");
  }

tb = table_open("score.txt");
  if(!tb){
	  return -1;
  }
	
  index = index_open("index.txt");
  if(!index){
	  table_close(tb);
	  return -1;
  }

  buff = (void **) malloc(NCOLS * sizeof(void *));
  if(!buff){
	  index_close(index);
	  table_close(tb);
	  return -1;
  }

  buff[0] = (long int *) malloc(sizeof(long int));
  if(!buff[0]){
	  index_close(index);
	  table_close(tb);
	  free(buff);
	  return -1;
  }
	
  buff[1] = (char *) malloc(MAX * sizeof(char));
  if(!buff[1]){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff);
	  return -1;
  }
	
  buff[2] = (int *) malloc(sizeof(int));
  if(!buff[2]){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff);
	  return -1;
  }
	
  buff[3] = (char *) malloc(MAX * sizeof(char));
  if(!buff[3]){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff);
	  return -1;
  }
	


  *((long long *) buff[0]) = user_id;
  strcpy((char *) buff[1], argv[1]);
  *((int *) buff[2]) = atoi(argv[2]);
  strcpy((char *) buff[3], comment);

  if(index_put(index, *((int *)buff[2]), table_last_pos(tb)) == -1){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff[3]);
	  free(buff);
	  return -1;
  }
	
  
  table_insert_record(tb, buff);
	
  
  if(index_save(index, "index.txt") == -1){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff[3]);
	  free(buff);
	  return -1;
  }


  free(buff[0]);
  free(buff[1]);
  free(buff[2]);
  free(buff[3]);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
	  index_close(index);
	  table_close(tb);
      return EXIT_FAILURE;
  }

  table_close(tb);
  index_close(index);

  free(buff);


	
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <unistd.h>

#include "table.h"
#include "type.h"
#include "odbc.h"
#include "index.h"

#define NCOLS 4
#define MAX 512

int main(int argc, char const *argv[]) {
  type_t types[NCOLS] = {LLNG, STR, INT, STR};
  table_t *tb = NULL;
  index_t *index = NULL;
  char query[MAX];
  char comment[MAX] = "\0";
  int i;
  void **buff = NULL;

  SQLBIGINT user_id;
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret;

  if(argc < 3){
    printf("\nError: introducir el nombre de usuario, puntuación y comentario\n");
    return -1;
  }

  for(i = 3;i < argc;i++){
      strcat(comment,argv[i]);
      strcat(comment," ");
  }

  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  sprintf(query, "select user_id from users where screenname = '%s'", argv[1]);

  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_SBIGINT, &user_id, sizeof(SQLBIGINT), NULL);
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
  }

  SQLCloseCursor(stmt);

  if( access("score.txt", F_OK ) != -1 ) {
  } else {
    table_create("score.txt", NCOLS, types);
  }

  if( access("index.txt", F_OK ) != -1 ) {
  } else {
    index_create(INT, "index.txt");
  }


  tb = table_open("score.txt");
  if(!tb){
	  return -1;
  }
	
  index = index_open("index.txt");
  if(!index){
	  table_close(tb);
	  return -1;
  }

  buff = (void **) malloc(NCOLS * sizeof(void *));
  if(!buff){
	  index_close(index);
	  table_close(tb);
	  return -1;
  }

  buff[0] = (long int *) malloc(sizeof(long int));
  if(!buff[0]){
	  index_close(index);
	  table_close(tb);
	  free(buff);
	  return -1;
  }
	
  buff[1] = (char *) malloc(MAX * sizeof(char));
  if(!buff[1]){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff);
	  return -1;
  }
	
  buff[2] = (int *) malloc(sizeof(int));
  if(!buff[2]){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff);
	  return -1;
  }
	
  buff[3] = (char *) malloc(MAX * sizeof(char));
  if(!buff[3]){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff);
	  return -1;
  }
	


  *((long long *) buff[0]) = user_id;
  strcpy((char *) buff[1], argv[1]);
  *((int *) buff[2]) = atoi(argv[2]);
  strcpy((char *) buff[3], comment);

  if(index_put(index, *((int *)buff[2]), table_cur_pos(tb)) == -1){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff[3]);
	  free(buff);
	  return -1;
  }
	
  
  table_insert_record(tb, buff);
	
  
  if(index_save(index, "index.txt") == -1){
	  index_close(index);
	  table_close(tb);
	  free(buff[0]);
	  free(buff[1]);
	  free(buff[2]);
	  free(buff[3]);
	  free(buff);
	  index_close(index);
	  table_close(tb);
	  return -1;
  }

  free(buff[0]);
  free(buff[1]);
  free(buff[2]);
  free(buff[3]);
  free(buff);

  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
	  index_close(index);
	  table_close(tb);
      return EXIT_FAILURE;
  }

  table_close(tb);
  index_close(index);



  return 0;
}
*/
  return 0;
}

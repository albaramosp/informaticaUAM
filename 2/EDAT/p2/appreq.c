#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#define LENGTH 50

int main(int argc, char const *argv[]) {
  char command[LENGTH];
  char param[LENGTH];
  char query[512];

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt, stmt2;
  SQLRETURN ret; /* ODBC API return status */

  SQLINTEGER followeed_num;
  SQLBIGINT followeed_id;
  SQLCHAR followeed_name[512];
  SQLINTEGER follower_num;
  SQLBIGINT follower_id;
  SQLCHAR follower_name[512];

  SQLINTEGER tweets_num;
  SQLBIGINT tweet_id;
  SQLCHAR tweet_text[512];
  SQLCHAR tweet_timestamp[512];

  SQLINTEGER retweets_num;
  SQLCHAR screenname[512];


  if(argc < 2){
    printf("\nError: introducir al menos el nombre del programa y un argumento\n");
    return -1;
  }

  strcpy(command, argv[1]);

  if(strcmp(command, "user") == 0){

    if(argc != 3){
      printf("Error en los argumentos: adjuntar solo el screenname del usuario");
      return -1;
    }

    strcpy(param, argv[2]);

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /*NUMERO DE USUARIOS SEGUIDOS*/

    sprintf(query, "select count(*) from follows, users where user_id = userfollower and screenname='%s'", param);
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SLONG, &followeed_num, sizeof(SQLINTEGER), NULL);

    printf("\n Numero de seguidos = ");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d\n", followeed_num);
    }

    SQLCloseCursor(stmt);

    /*LISTA DE USUARIOS SEGUIDOS*/

    sprintf(query, "select user_id, screenname from users where user_id in (select userfolloweed from follows where userfollower in (	select user_id from users where screenname = '%s'))", param);

    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SBIGINT, &followeed_id, sizeof(SQLBIGINT), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, followeed_name, sizeof(followeed_name), NULL);

    printf("\n Lista de seguidos:\n");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%ld \t %s\n", followeed_id, followeed_name);
    }

    SQLCloseCursor(stmt);



    /*NUMERO DE SEGUIDORES*/
    sprintf(query, "select count(*) from follows where userfolloweed in (select user_id from users where screenname = '%s' );", param);
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SLONG, &follower_num, sizeof(SQLINTEGER), NULL);

    printf("\n Numero de seguidores = ");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d\n", follower_num);
    }

    SQLCloseCursor(stmt);


    /*LISTA DE SEGUIDORES*/
    sprintf(query, "select user_id, screenname from users where user_id in (select userfollower from follows where userfolloweed in (	select user_id from users where screenname = '%s'))", param);
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SBIGINT, &follower_id, sizeof(SQLBIGINT), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, follower_name, sizeof(follower_name), NULL);

    printf("\n Lista de seguidores:\n");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%ld \t %s\n", follower_id, follower_name);
    }

    SQLCloseCursor(stmt);


    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
  } /*close 'user' case*/

  if(strcmp(command, "tweets") == 0){
    if(argc != 3){
      printf("Error en los argumentos: adjuntar solo el screenname del usuario");
      return -1;
    }

    strcpy(param, argv[2]);

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /*NUMERO DE TWEETS*/
    sprintf(query, "select count(*) from tweets where userwriter in ( select user_id from users where screenname='%s')", param);
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
    sprintf(query, "select tweet_id, tweettimestamp, tweettext from tweets, users where userwriter=user_id and screenname='%s' order by tweettimestamp asc", param);

    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SBIGINT, &tweet_id, sizeof(SQLBIGINT), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, tweet_timestamp, sizeof(tweet_timestamp), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, tweet_text, sizeof(tweet_text), NULL);

    printf("\n Lista de tweets:\n");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%ld \t %s \t %s\n", tweet_id, tweet_timestamp, tweet_text);
    }

    SQLCloseCursor(stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }


  }/*close 'tweets' option*/

  if(strcmp(command, "retweets") == 0){
    if(argc != 3){
      printf("Error en los argumentos: agregar solo el id del tweet");
      return -1;
    }

    strcpy(param, argv[2]);

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /*NUMERO DE RETWEETS*/
    sprintf(query, "select count(*) from tweets where retweet = %s", param);
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SLONG, &retweets_num, sizeof(SQLINTEGER), NULL);

    printf("\n Numero de retweets = ");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%d\n", retweets_num);
    }

    SQLCloseCursor(stmt);

    /*LISTA DE RETWEETS*/
    sprintf(query, "select tweet_id, screenname, tweettimestamp, tweettext from tweets, users where tweets.userwriter = users.user_id and retweet = %s order by tweettimestamp", param);
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SBIGINT, &tweet_id, sizeof(SQLBIGINT), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, screenname, sizeof(screenname), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, tweet_timestamp, sizeof(tweet_timestamp), NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, tweet_text, sizeof(tweet_text), NULL);

    printf("\n Lista de retweets:\n\n");
    printf("tweet_id \t\t Screenname \t tweet_timestamp \t\t tweet_text\n");

    /* Loop through the rows in the result-setretweets_num */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%ld \t %s \t %s \t %s\n", tweet_id, screenname, tweet_timestamp, tweet_text);
    }

    SQLCloseCursor(stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

  }

  if(strcmp(command, "maxrt") == 0){

    if(argc != 2){
      printf("\nError: no introducir mas argumentos para maxrt \n");
      return -1;
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt2);

    /*RANKING DE 20 TWEETS MAS RETWEETEADOS*/
    sprintf(query, "select count(*) retweet, retweet from tweets where retweet is not null group by retweet order by count(*) desc limit 20");
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SLONG, &retweets_num, sizeof(SQLINTEGER), NULL);
    SQLBindCol(stmt, 2, SQL_C_SBIGINT, &tweet_id, sizeof(SQLBIGINT), NULL);

    printf("\n num_retweets \t tweet_id \t\t screenname\n");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      sprintf (query, "select screenname from users where user_id = (select userwriter from tweets where tweet_id = %ld)", tweet_id);
      /*printf("\nQuery: %s\n", query);*/
      SQLExecDirect(stmt2, (SQLCHAR*) query, SQL_NTS);
      SQLBindCol(stmt2, 1, SQL_C_CHAR, screenname, sizeof(screenname), NULL);
      while (SQL_SUCCEEDED(ret = SQLFetch(stmt2))) {
        printf(" %d\t %ld\t\t %s \n", retweets_num, tweet_id, screenname);
      }
      SQLCloseCursor(stmt2);

    }

    SQLCloseCursor(stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

  }

  if(strcmp(command, "maxfw") == 0){

    if(argc != 2){
      printf("\nError: no introducir mas argumentos para maxfw \n");
      return -1;
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /*RANKING DE 20 USUARIOS MAS SEGUIDOS*/
    sprintf(query, "select user_id, screenname, count (*) as total_seguidores from follows, users where userfolloweed=user_id group by user_id order by total_seguidores desc limit 20");
    /*printf("\nQuery: %s\n", query);*/

    SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

    SQLBindCol(stmt, 1, SQL_C_SBIGINT, &followeed_id, sizeof(SQLBIGINT), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, screenname, sizeof(screenname), NULL);
    SQLBindCol(stmt, 3, SQL_C_SLONG, &follower_num, sizeof(SQLINTEGER), NULL);

    printf("\n user_id \t screenname \t total_seguidores\n");

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%ld \t %s \t %d\n", followeed_id, screenname, follower_num);
    }


    SQLCloseCursor(stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
  }

  return 0;
}

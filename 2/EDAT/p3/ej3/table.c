#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "type.h"

/*
--DEPURACION DE FICHEROS BINARIOS--
hd
xxd
hexdump
*/

struct table_ {
  type_t* types;
  char* path;
  int ncols;
  long first_pos;
  long cur_pos;
  long last_pos;
  void** values;
};

/*
   Creates a file that stores an empty table. This function doesn't
   keep any information in memory: it simply creates the file, stores
   the header information, and closes it.
*/
void table_create(char* path, int ncols, type_t* types) {
  /* To be implemented
  fopen del fichero argumento path
  fwrite del numero de columnas y los tipos
  fclose
  */

  FILE *f;

  f = fopen(path, "wb");

  fseek(f, 0, SEEK_SET); /*apunto al principio*/
  fwrite(&ncols, sizeof(int), 1, f);

  fwrite(types, sizeof(type_t), ncols, f);

  fclose(f);

  return;
}

/*
   Opens a table given its file name. Returns a pointer to a structure
   with all the information necessary to manage the table. Returns
   NULL is the file doesn't exist or if there is any error.
*/
table_t* table_open(char* path) {
  /* To be implemented
  malloc de la estructura
  fopen rb
  fread del numero de columnas y tipos
  actualizar la posicion del puntero de la cabecera (campo first_pos)
  fclose
  devolver la variable de tipo table
  */
  table_t* t = NULL;
  FILE* f = NULL;
  int i;

  t = (table_t *) malloc(sizeof(table_t));

  f = fopen(path, "rb");
  if(f == NULL) return NULL;

  fseek(f, 0, SEEK_SET);
  fread(&(t->ncols), sizeof(int), 1, f);



  t->types = (type_t *) malloc(sizeof(int) * t->ncols);

  fread(t->types, sizeof(int), t->ncols, f);

  t->cur_pos = ftell(f);

  t->first_pos = ftell(f);

  fseek(f, 0, SEEK_END);

  t->last_pos = ftell(f);

  t->path = (char *) malloc(sizeof(char) * (strlen(path) + 1));
  strcpy(t->path, path);

  t->values = (void **) calloc(t->ncols, sizeof(void *) );


  for(i = 0;i < t->ncols;i++){
    if(t->types[i] == INT){
      t->values[i] = (int *) malloc(sizeof(int));
    }
    else if(t->types[i] == STR){
      t->values[i] = (char *) malloc(sizeof(char));
    }
    else if(t->types[i] == LLNG){
      t->values[i] = (long long int *) malloc(sizeof(long long int));
    }
    else if(t->types[i] == DBL){
      t->values[i] = (double *) malloc(sizeof(double));
    }
  }

  fclose(f);

  return t;
}

/*
   Closes a table freeing the alloc'ed resources and closing the file
   in which the table is stored.
*/
void table_close(table_t* table) {
  /* To be implemented*/
  int i;
  if(!table) return;
	
  for(i = 0; i < table->ncols; i++){
    free(table->values[i]);
    table->values[i] = NULL;
  }
	
  free(table->values);
  table->values = NULL;
	
  free(table->path);
  table->path = NULL;
	
  free(table->types);
  table->types = NULL;
	
  free(table);
  table= NULL;
  return;
}

/*
   Returns the number of columns of the table
*/
int table_ncols(table_t* table) {
  if(!table) return -1;
  return table->ncols;
}

/*
   Returns the array with the data types of the columns of the
   table. Note that typically this kind of function doesn't make a
   copy of the array, rather, it returns a pointer to the actual array
   contained in the table structure. This means that the calling
   program should not, under any circumstance, modify the array that
   this function returns.
 */
type_t* table_types(table_t* table) {
  /* To be implemented */
  if(!table) return NULL;
  return table->types;
}

/*
   Returns the position in the file of the first record of the table
*/
long table_first_pos(table_t* table) {
  if(!table) return 0L;
  return table->first_pos;
  /*FILE * pf = NULL;
  pf = fopen(table->path, "r");
  if(fseek(pf, 1, SEEK_SET) != 0)
    return ftell(pf);

  fclose(pf);*/
}

/*
   Returns the position in the file in which the table is currently
   positioned.
*/
long table_cur_pos(table_t* table) {
  if(!table) return 0L;
  return table->cur_pos;

  /*FILE* pf = NULL;
  pf = fopen(table->path, "r");
  if(fseek(pf, 1, SEEK_CUR) != 0)
    return ftell(pf);

  fclose(pf);*/
}

/*
   Returns the position just past the last byte in the file, where a
   new record should be inserted.
*/
long table_last_pos(table_t* table) {
  if(!table) return 0L;
  return table->last_pos;
  /*FILE* pf = NULL;
  pf = fopen(table->path, "r");
  if(fseek(pf, 1, SEEK_END) != 0)
    return ftell(pf);*/
}

/*
   Reads the record starting in the specified position. The record is
   read and stored in memory, but no value is returned. The value
   returned is the position of the following record in the file or -1
   if the position requested is past the end of the file.
*/
long table_read_record(table_t* table, long pos) {
  /* To be implemented */
  FILE* f = NULL;
  int i;
  int aux;

  if(!table) return -1L;

  f = fopen(table->path, "rb");
  if(!f) return -1;



  fseek(f, 0, SEEK_END);
  table->last_pos = ftell(f);

  fseek(f, pos, SEEK_SET);
  table->cur_pos = ftell(f);

  if(table->cur_pos == table->last_pos){
    fclose(f);
    return -1;
  }

  for(i = 0;i < table->ncols;i++){
    if(table->types[i] == STR){
      fread(&aux, sizeof(int), 1, f);
      free(table->values[i]);
      table->values[i] = (char *) malloc(sizeof(char) * (aux + 1));
      fread(table->values[i],  sizeof(char) * (aux + 1), 1, f);
    }else{
      if(table->types[i] == INT){
        fread(table->values[i], sizeof(int), 1, f);
      }
      if(table->types[i] == DBL){
        fread(table->values[i], sizeof(double), 1, f);
      }
      if(table->types[i] == LLNG){
        fread(table->values[i], sizeof(long long int), 1, f);
      }
    }
  }
  table->cur_pos = ftell(f);
  fclose(f);

  return table->cur_pos;


}

/*
  Returns a pointer to the value of the given column of the record
  currently in memory. The value is cast to a void * (it is always a
  pointer: if the column is an INT, the function will return a pointer
  to it).. Returns NULL if there is no record in memory or if the
  column doesn't exist.
*/
void *table_column_get(table_t* table, int col) {
  /* To be implemented */
  if(!table || !(table->values[col])) return NULL;

  return (void *)(table->values[col]);
}


/*
   Inserts a record in the last available position of the table. Note
   that all the values are cast to void *, and that there is no
   indication of their actual type or even of how many values we ask
   to store... why?
  */
void table_insert_record(table_t* table, void** values) {
  /* To be implemented */
  int i, aux;
  FILE* f = NULL;

  if(!table || !values) return;

  f = fopen(table->path, "ab");
  if(!f) return;

  fseek(f, table->cur_pos, SEEK_SET);

  for(i = 0;i < table->ncols;i++){
    if(table->types[i] == STR){
      aux = strlen(values[i]);
      /*free(table->values[i]);
      table->values[i] = (char *) malloc(sizeof(char) * (strlen(values[i]) + 1));
      strcpy(table->values[i], values[i]);*/
      fwrite(&aux, sizeof(int), 1, f);
      fwrite(values[i], (aux + 1) * sizeof(char), 1, f);
    }else{
      /*table->values[i] = values[i];*/
      if(table->types[i] == INT){
        fwrite(values[i], sizeof(int), 1, f);
      }
      if(table->types[i] == DBL){
        fwrite(values[i], sizeof(double), 1, f);
      }
      if(table->types[i] == LLNG){
        fwrite(values[i], sizeof(long long int), 1, f);
      }
    }
  }

  table->cur_pos = ftell(f);
  table->last_pos = table->cur_pos;

  fclose(f);
  return;
}

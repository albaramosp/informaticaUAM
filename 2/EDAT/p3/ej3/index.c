#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"
#include "type.h"

typedef struct {
  int key;
  long *direcciones;
  int n_regs;
} irecord;

struct index_ {
    char *path;
    irecord **claves;
    int entries;
    int type;
};

int bbin(irecord **tabla,int P,int U,int clave,int *ppos);


/*
   Creates a file for saving an empty index. The index is initialized
   to be of the specific tpe (in the basic version this is always INT)
   and to contain 0 entries.
 */
int index_create(int type, char* path) {
  FILE *f;
  int entries = 0;

  if(type != INT){
    return -1;
  }

  f = fopen("index.txt", "wb");

  /*escribimos el tipo de indice y el numero de registros que hay escritos en el*/
  fwrite(&type, sizeof(int), 1, f);
  fwrite(&entries, sizeof(int), 1, f);

  fclose(f);

  return 0;
}

/*
   Opens a previously created index: reads the contents of the index
   in an index_t structure that it allocates, and returns a pointer to
   it (or NULL if the files doesn't exist or there is an error).

   NOTE: the index is stored in memory, so you can open and close the
   file in this function. However, when you are asked to save the
   index, you will not be given the path name again, so you must store
   in the structure either the FILE * (and in this case you must keep
   the file open) or the path (and in this case you will open the file
   again).
 */
index_t* index_open(char* path) {
  index_t *index;
  int i, j;
  FILE* f;

  if(!path){
    return NULL;
  }

  index = (index_t *)malloc(sizeof(index_t));
  if(!index){
    return NULL;
  }

  index->path = (char *)malloc(sizeof(char) * (strlen(path)+1) );
  if(!(index->path)){
    free(index);
    return NULL;
  }

  strcpy(index->path, path);

  f = fopen(path, "rb");
	if(!f){
		free(index->path);
		free(index);
		return NULL;
  }

  fseek(f, 0, SEEK_SET);

  /*read type of index*/
  fread(&(index->type), sizeof(int), 1, f);

  /*position of num_entries for further editions*/
  fread(&(index->entries), sizeof(int), 1, f);

  index->claves = (irecord **)malloc(index->entries * sizeof(irecord *));
  for(i = 0;i < index->entries;i++){
    index->claves[i] = (irecord *) malloc(sizeof(irecord));
  }

  for(i = 0; i < index->entries; i++){
    fread(&(index->claves[i]->key), sizeof(int), 1, f);
    fread(&(index->claves[i]->n_regs), sizeof(int), 1, f);

    index->claves[i]->direcciones = (long *) malloc(sizeof(long) * index->claves[i]->n_regs);
    for(j = 0;j < index->claves[i]->n_regs;j++){
      fread(&(index->claves[i]->direcciones[j]), sizeof(long), 1, f);
    }
  }

  fclose(f);
  return index;
}

/*
   Closes the index by freeing the allocated resources
*/
void index_close(index_t *index) {
  int i, j;

  for(i = 0; i < index->entries; i++){
      free(index->claves[i]->direcciones);
	  free(index->claves[i]);
  }

  free(index->path);
  free(index->claves);
  free(index);

}

/*
   Saves the current state of index in the file it came from. See the
   NOTE to index_open.
*/
int index_save(index_t* index, char* path) {
  FILE* f = NULL;
  int i, j;

  f = fopen(path, "wb");
  if(!f || !index || !path) return -1;

  fseek(f, 0, SEEK_SET);

  fwrite(&(index->type), sizeof(int), 1, f);
  fwrite(&(index->entries), sizeof(int), 1, f);

  for(i = 0;i < index->entries;i++){
    fwrite(&(index->claves[i]->key), sizeof(int), 1, f);
    fwrite(&(index->claves[i]->n_regs), sizeof(int), 1, f);

    for(j = 0;j < index->claves[i]->n_regs;j++){
      fwrite(&(index->claves[i]->direcciones[j]), sizeof(long), 1, f);
    }
  }

  fclose(f);

  return 0;
}

long **index_get_edited(index_t *index, int key, int* nposs);
int bbin(irecord **tabla,int P,int U,int clave,int *ppos);
long **bbin_custom(irecord **tabla,int P,int U,int clave,int *ppos);

/*
   Puts a pair key-position in the index. Note that the key may be
   present in the index or not... you must manage both situation. Also
   remember that the index must be kept ordered at all times.
*/
int index_put(index_t *index, int key, long pos) {
  int j, pos_clave;
  irecord *aux = NULL;

  index_get_edited(index, key, &pos_clave);

  if(pos_clave == -1){
    index->entries++;
    index->claves = (irecord **) realloc(index->claves, sizeof(irecord*) * index->entries);
    index->claves[index->entries-1] = (irecord *) malloc(sizeof(irecord));

    index->claves[index->entries-1]->key = key;
    index->claves[index->entries-1]->n_regs = 1;
    index->claves[index->entries-1]->direcciones = (long *) malloc(sizeof(long));
    *(index->claves[index->entries-1]->direcciones) = pos;

    for(j = index->entries - 1;j > 0;j--){
      if(index->claves[j]->key < index->claves[j - 1]->key){
        aux = index->claves[j];
        index->claves[j] = index->claves[j - 1];
        index->claves[j - 1] = aux;
      }
    }

    return 0;
  }

  index->claves[pos_clave]->n_regs++;
  index->claves[pos_clave]->direcciones = (long *) realloc(index->claves[pos_clave]->direcciones, sizeof(long) * index->claves[pos_clave]->n_regs);
  index->claves[pos_clave]->direcciones[index->claves[pos_clave]->n_regs - 1] = pos;

  return 0;
}

long **index_get(index_t *index, int key, int* nposs) {
  int p, u;


  if(!index || !nposs || key < 0){
    return NULL;
  }

  p = 0;
  u = (index->entries)-1;
  if(u == -1){
    *nposs = 0;
	return NULL;
  }

  return bbin_custom(index->claves, p, u, key, nposs);
}

long **bbin_custom(irecord **tabla,int P,int U,int clave,int *ppos){
  int m, i;
  long **directions = NULL;

  if( !tabla || P < 0 || P > U || clave < 0 || !ppos ){
    return NULL;
  }

  while( P <= U ){
    m=(P+U)/2;
    if(tabla[m]->key == clave){
      *ppos = tabla[m]->n_regs;
      directions = (long **) malloc(sizeof(long *));
      for(i = 0;i < tabla[m]->n_regs;i++){
        directions[i] = (long *) malloc(sizeof(long));
        *(directions[i]) = tabla[m]->direcciones[i];
      }
      return directions;
    }
	  else if(tabla[m]->key > clave){
      U = m-1;
    }
	  else {
      P=m+1;
    }
  }

  *ppos = 0;/*clave no encontrada*/
  return NULL;
}

/*
   Retrieves all the positions associated with the key in the index.

   NOTE: the parameter nposs stores the number of positions associated to the key. The call
   will be something like this:

   int n
   long **poss = index_get(index, key, &n);

   for (int i=0; i<n; i++) {
       Do something with poss[i]
   }
*/

long **index_get_edited(index_t *index, int key, int* nposs) {
  int p, u;


  if(!index || !nposs || key < 0){
    return NULL;
  }

  p = 0;
  u = (index->entries)-1;
  if(u == -1){
    *nposs = -1;
  }
  if(bbin(index->claves, p, u, key, nposs) == -1){
    return NULL;
  }


  return NULL;
}

int bbin(irecord **tabla,int P,int U,int clave,int *ppos){
  int obs,m;
  obs =0;

  if( !tabla || P < 0 || P > U || clave < 0 || !ppos ){
    return -1;
  }

  while( P <= U ){
    m=(P+U)/2;
    if(tabla[m]->key == clave){
      *ppos = m;
      obs++;
      return obs;
    } else if(tabla[m]->key > clave){
      U = m-1;
    }else {
      P=m+1;
    }
    obs++;
  }

  *ppos = -1;
  return obs;
}

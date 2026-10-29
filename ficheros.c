#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ficheros.h"

int checkAcceso(char *nombre, char *modo){ //funcion para chequear si tenemos acceso al archivo
  FILE *file = fopen(nombre, modo);
  if(file==NULL){
    return NO_EXISTE;
  };
  fclose(file);
  return EXISTE;
};

int guardar_procesos(char *nombre, item_t *items, int count){ //guardar procesos en el archivo
  errno=0;
  FILE *file = fopen(nombre, "wb");
  if(file==NULL){ //si no podemos abrirlo por lo que sea, lo cerramos y avisamos
    return ERROR_AL_ABRIR;
  };

  if(fwrite(&count, sizeof(int), 1, file)!=1){ //lo mismo si hay error en la escritura
    fclose(file);
    return ERROR_WRITE;
  };

  for (int i = 0; i < count; i++) { //escribimos cada proceso usando un for
    item_file_t record;
    strncpy(record.user, items[i].user, sizeof(record.user) - 1);
    record.user[sizeof(record.user) - 1] = '\0';
    record.p = items[i].p;
    record.c = items[i].c;
    record.state = items[i].state;

    if (fwrite(&record, sizeof(item_file_t), 1, file) != 1) {
      fclose(file);
      return ERROR_WRITE;
    };

    fclose(file);
    return SUCCESS;
  };
};

int cargar_procesos(char *nombre, item_file_t *items, int maximo, int *outcount){ //cargar procesos dado un archivo
  errno = 0;
    FILE *file = fopen(nombre, "rb"); //esto es igual que la funcion anterior
    if (file == NULL) {
        return ERROR_AL_ABRIR;
    };

    int count = 0;
    // Leer número total de elementos almacenados
    if (fread(&count, sizeof(int), 1, file) != 1) { //same thing still :D
        fclose(file);
        return ERROR_READ;
    };

    if (count > maximo) { //si hay mas procesos de los que podemos meter damos error
        fclose(file);
        return ERROR_READ;
    };

    // Leer cada registro de proceso
    for (int i = 0; i < count; i++) {
        if (fread(&items[i], sizeof(item_file_t), 1, file) != 1) {
            fclose(file);
            return ERROR_READ;
        };
    };

    *outcount = count;
    fclose(file);
    return SUCCESS;
};
    

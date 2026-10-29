
#ifndef FICHEROS_H
#define FICHEROS_H

#include <stddef.h>
#include "item.h"

/* Códigos de retorno para las operaciones con ficheros */
#define SUCCESS         0   // Operación realizada con éxito
#define ERROR_AL_ABRIR       -1   // No se pudo abrir el fichero (no existe, sin permisos, etc.)
#define ERROR_WRITE      -2   // Error al escribir con fwrite
#define ERROR_READ       -3   // Error al leer con fread
#define EXISTE          1   // El fichero ya existe (para confirmar sobreescritura)
#define NO_EXISTE      2   // El fichero no existe

typedef struct { 
    char user[100];         /* Mismo tamaño máximo que en item.h */
    int p;                  /* Prioridad */
    int c;                  /* Tiempo de ejecución */
    process_state_t state;  /* Estado */
} item_file_t;

int check_access(const char *filename, const char *mode);

int guardar_procesos(const char *filename, const item_t *items, int count);

int cargar_procesos(const char *filename, item_file_t *out_items, int max_capacity, int *out_count);

#endif

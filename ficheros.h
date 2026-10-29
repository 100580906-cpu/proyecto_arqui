#ifndef FICHEROS_H
#define FICHEROS_H

#include <stddef.h>
#include "item.h"

/* Códigos de retorno para las operaciones con ficheros */
#define SUCCESS         0   /* Operación realizada con éxito */
#define ERROR_AL_ABRIR       -1   /* No se pudo abrir el fichero (no existe, sin permisos, etc.) */
#define ERROR_WRITE      -2   /* Error al escribir con fwrite */
#define ERROR_READ       -3   /* Error al leer con fread */
#define EXISTE          1   /* El fichero ya existe (para confirmar sobreescritura) */
#define NO_EXISTE      2   /* El fichero no existe */

/* Estructura para almacenamiento en disco: exime el PID según el enunciado */
typedef struct {
    char user[100];         /* Mismo tamaño máximo que en item.h */
    int p;                  /* Prioridad */
    int c;                  /* Tiempo de ejecución */
    process_state_t state;  /* Estado */
} item_file_t;

/**
 * Comprueba si un fichero existe y si se tienen permisos para leerlo o escribirlo.
 * 
 * @param filename Nombre o ruta del fichero.
 * @param mode "r" para comprobar lectura, "w" para comprobar escritura.
 * @return FILE_EXISTS si existe y se tiene acceso, FILE_NOT_EXISTS si no existe o no hay permisos.
 */
int file_check_access(const char *filename, const char *mode);

/**
 * Guarda los procesos de la colección en un fichero binario usando fwrite.
 * NO guarda el PID de ningún proceso.
 *
 * @param filename Nombre del fichero destino.
 * @param items Array de procesos a guardar.
 * @param count Número total de procesos.
 * @return FILE_SUCCESS si ok, FILE_ERR_OPEN o FILE_ERR_WRITE si falla.
 */
int file_save_collection(const char *filename, const item_t *items, int count);

/**
 * Lee los procesos desde un fichero binario usando fread.
 *
 * @param filename Nombre del fichero origen.
 * @param out_items Array donde se almacenarán los datos leídos.
 * @param max_capacity Capacidad máxima del array out_items.
 * @param out_count Puntero donde se devolverá el número de elementos leídos.
 * @return FILE_SUCCESS si ok, FILE_ERR_OPEN o FILE_ERR_READ si falla.
 */
int file_load_collection(const char *filename, item_file_t *out_items, int max_capacity, int *out_count);

#endif

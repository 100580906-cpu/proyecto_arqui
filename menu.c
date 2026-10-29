/*
 * Fichero: menu.c
 * Descripción: Programa que implementa las funciones declaradas en menu.h
 * Arquitectura de Sistemas - UC3M - Irene Rossi Albarrán
 */


#include <stdio.h>
#include "menu.h"

void mostrar_menu(void){
  printf("Seleccione una de las siguientes opciones:\n");
  printf("1.Añadir un nuevo proceso a la planificación ");
  printf("2.Borrar un proceso determinado de la planificación.");
  printf("3.Información acerca de un proceso.");
  printf("4.Mostrar toda la planificación.");
  printf("5.Borrar la planificación actual.");
  printf("6.Ordenar la planificación según un criterio.");
  printf("7.Ayuda.");
  printf("8.Salir.");
}

void mostar_ayuda(void){
  printf("AYUDA DEL PLANIFICADOR\n");
  printf("1.Añadir un nuevo proceso.");
  printf("2.Borrar un proceso existente.");
  printf("3.Consultar información de un proceso.");
  printf("4.Mostrar todos los procesos.");
  printf("5.Borrar todos los procesos de la planificación.");
  printf("6.Ordenar los procesos.");
  printf("7.Mostar esta ayuda.");
  printf("8.Salir del programa.");
}

int devolver_option(void) {
    int option = 0;
    printf("Por favor, introduzca una opción (ayuda: 10): ");
    fflush(stdout);

    int status = read_int(&option);

    if (status == INPUT_EOF) {
        return INPUT_EOF;
    }
    if (status == INPUT_INVALID || option < 0 || option > 10) {
        return INPUT_INVALID;
    }

    return option;
}
este el de menu.c y ya estaria

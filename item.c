#include "item.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_item(struct item* foo){
  char *nombres_estado[] = {"READY", "RUNNING", "STOPPED", "TERMINATED"};

  printf("%5d ", foo->pid);
  printf("%5u ", foo->P);
  printf("%5u ", foo->C);
  printf("%10s ", nombres_estado[foo->estado]);
  printf("%-20s\n", foo->user);
}

struct item *create_item(char *nombre, unsigned int prioridad, unsigned int tiempo, int *contador_pid){
  struct item *nuevo;
  nuevo = malloc(sizeof(struct item));

  if (nuevo == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  nuevo->user = strdup(nombre);
  if (nuevo->user == NULL) {
    errno = ENOMEM;
    free(nuevo);
    return NULL;
  }
  nuevo->pid = *contador_pid;
  (*contador_pid)++;

  nuevo->P = prioridad;
  nuevo->C = tiempo;
  nuevo->estado = READY;
  return nuevo;
}

void free_item(struct item *foo){
  if (foo != NULL) {
    free(foo->user);
    free(foo);
  }
}

int compare_PID(const void *pa, const void *pb){
  struct item *a = (struct item *) pa;
  struct item *b = (struct item *) pb;
  if(a->pid == b->pid) return 0;
  if(a->pid < b->pid) return -1;
  return 1;
}

int compare_P(const void *pa, const void *pb){
  struct item *a = (struct item *) pa;
  struct item *b = (struct item *) pb;
  if(a->P == b->P) return 0;
  if(a->P < b->P) return -1;
  return 1;
}

int compare_C(const void *pa, const void *pb){
  struct item *a = (struct item *) pa;
  struct item *b = (struct item *) pb;
  if(a->C == b->C) return 0;
  if(a->C < b->C) return -1;
  return 1;
}

int compare_OWNER(const void *pa, const void *pb){
  struct item *a = (struct item *) pa;
  struct item *b = (struct item *) pb;
  return (strcmp(a->user, b->user));
}

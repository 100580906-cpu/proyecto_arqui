/******************************************************************
 Autor: Violeta Pérez Chávarri

*******************************************************************/

#ifndef ITEM_H
#define ITEM_H

typedef enum {READY, RUNNING, STOPPED, TERMINATED} state_t;

struct item{
  char *user;
  int pid;
  unsigned int P;
  unsigned int C;
  state_t estado;
};

struct item *create_item(char *, unsigned int, unsigned int, int *);

void free_item(struct item*);

void print_item(struct item*);

int compare_PID(const void *, const void *);
int compare_P(const void *, const void *);
int compare_C(const void *, const void *);
int compare_OWNER(const void *, const void *);

#endif

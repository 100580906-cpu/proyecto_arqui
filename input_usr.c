#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int read_int(*int result){
  ssize_t number = 0;
  char *mystr = NULL;
  size_t n_alloc = 0;
  long val;
  char *endptr;
  errno = 0;

  number = getline(&mystr,&n_alloc,stdin); //leemos la cadena con el getline
  if(number == -1){ //en caso de que haya un error errno liberamos la memoria y retornamos EOF
    free(mystr);
    return INPUT_EOF;
  };

  val= strtol(mystr,&endptr,10);
  if(*endptr!='\0'||endptr==mystr||val == LONG_MAX || val = LONG_MIN){ //en caso de que ocurra alguno de estos casos, la cadena no se puede admitir
    free(mystr);   //los casos en orden son: hay algun caracter no numerico || no se detectan numeros || es un numero demasiado grande || es demasiado pequeño
    return(mystr);
  };
  *result = (int)val; //si esta todo bien, devolvemos el numero como int y liberamos la memoria
  free(mystr);
  return INPUT_OK;
};

int read_string(char *result){
    ssize_t number = 0;
    char *mystr = NULL;
    size_t n_alloc = 0;
    char *endptr;
    errno = 0;

    number = getline(&mystr,&n_alloc,stdin); //leemos con getline
    if(number == -1){ //en caso de error liberamos memoria
      free(mystr);
      return INPUT_EOF;
    };
    strcpy(result,mystr); //copiamos lo que hemos leido al resultado
    free(mystr);
    return INPUT_OK;
};

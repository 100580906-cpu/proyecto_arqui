#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include "input_usr.h"

int read_int(int *result){
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

if(number > 0 && mystr[number-1] == '\n'){ //debemos eliminar el salto de línea ya que el getline también almacena ese carácter que no necesitamos. Nosotros solo queremos lo que escriba el usuario.
	mystr[number-1] = '\0';
};
	
  errno = 0; //resetamos errno a 0 por si hay algún error antiguo
  val= strtol(mystr,&endptr,10);
  if(*endptr!='\0'||endptr==mystr||errno == ERANGE|| val > INT_MAX || val < INT_MIN){ //en caso de que ocurra alguno de estos casos, la cadena no se puede admitir
    free(mystr);   //los casos en orden son: hay algun caracter no numerico || no se detectan numeros || es un numero demasiado grande || es demasiado pequeño. 
				//El ERANGE al parecer es necesario para ver si es mas grande o mas pequeño
    return INPUT_INVALID;
  };
  *result = (int)val; //si esta todo bien, devolvemos el numero como int y liberamos la memoria
  free(mystr);
  return INPUT_OK;
};

int read_string(char *result, size_t max_size){ //le pasamos el tamaño máximo permitido por si acaso el user mete algo demasiado largo
    ssize_t number = 0;
    char *mystr = NULL;
    size_t n_alloc = 0;

    number = getline(&mystr,&n_alloc,stdin); //leemos con getline
    if(number == -1){ //en caso de error liberamos memoria
      free(mystr);
      return INPUT_EOF;
    };

    if(number >0 && mystr[number-1] == '\n'){ //borramos el salto de linea
	mystr[number-1] = '\0';
    };
	
    strncpy(result,mystr, max_size-1); //copiamos lo que hemos leido al resultado !!!! HAY QUE USAR strNcpy para poder pasarle el max_size, el strcpy normal solo acepta dos parámetros.
    result[max_size-1] = '\0';
    free(mystr);
    return INPUT_OK;
};

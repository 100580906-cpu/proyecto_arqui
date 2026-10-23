//valores para casos de errores en la lectura
#define INPUT_OK   0 //sin errores
#define INPUT_EOF  -1 //error tipo errno
#define INPUT_INVALID -2 //cadena invalida
int read_string(char *result);
int read_int(int *result);

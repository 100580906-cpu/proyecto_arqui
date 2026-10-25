//valores para casos de errores en la lectura
#define INPUT_OK   0 //sin errores
#define INPUT_EOF  -1 //Detectado Cntrl+D
#define INPUT_INVALID -2 //cadena invalida
int read_string(char *result, size_t max_size); //result = donde se copiara el texto del usuario | max_size = tamaño maximo del resultado
                                                //retorna INPUT_OK si la lectura es correcta, INPUT_EOF si se pulsa CTRL+D
int read_int(int *result); //result = Puntero donde se almacenará el número leído
                           //retorna INPUT_OK si la lectura es correcta, INPUT_EOF si se pulsa CTRL+D

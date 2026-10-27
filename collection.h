struct collection add_item(struct collection col,
			   char *nombre, unsigned int p, unsigned int C, int *error){

  errno = 0;
  *error = SUCCESS;
  
  if (col.number_occupied == col.total_capacity){
    if (col.total_capacity == 0){ // Not initialized
      col = initialize_collection(error);
      if (error)
	return col;
    }else{ // reached total capacity
      int new_cap = col.total_capacity *2;
      size_t pedir = new_cap * sizeof(struct item*);
      struct item **tmp;
      tmp = (struct item **) realloc (col.array, pedir);
      if (tmp == NULL){
	*error = errno;
	return col;
      }
      col.array = tmp;
      col.total_capacity = new_cap;
    }
  }

col.array[col.number_occupied] = create_item(nombre, p, C, &col.next_pid);
  if (col.array[col.number_occupied] == NULL){
    *error = errno;
    return col;
}
  col.number_occupied++;
  return col;
}
struct collection delete_item(struct collection col,
			      int pid, int *errnum){
  *errnum = SUCCESS;
  if (col.number_occupied == 0){
    *errnum = ERR_NO_ITEMS;
    return col;
  }
  
  int index = find_item(col, pid, errnum);
  if ((index == -1) || (*errnum)) {
    *errnum = ELEMENT_NOT_FOUND;
    return col;
  }

  free_item(col.array[index]);
  col.number_occupied--;

  if ((col.number_occupied) && (index != col.number_occupied)){
    // copy the last one here
    memcpy(&(col.array[index]),&(col.array[col.number_occupied]),
	   sizeof(struct item));
  }
  return col;
}

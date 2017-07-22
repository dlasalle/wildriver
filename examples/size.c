#include <wildriver.h>
#include <stdio.h>

int main(
  int argc,
  char ** argv)
{
  int rc;
  wildriver_matrix_handle * handle;  
  char * filename;
  size_t * rowptr;
  uint32_t * ind;
  float * val; 

  filename = argv[1];

  rc = 0;
  if ((handle = wildirver_open_matrix(filename, WILDRIVER_IN)) == NULL) {
    printf("Failed to open '%s'.", filename);
    rc = 1;
    goto CLEANUP;
  }

  if ((rowptr = malloc(sizeof(*rowptr)*(handle->nrows+1))) == NULL) {
    printf("Failed to allocate 'rowptr'.");
    rc = 1;
    goto CLEANUP;
  }

  if ((ind = malloc(sizeof(*ind)*(handle->nnz))) == NULL) {
    printf("Failed to allocate 'ind'.");
    rc = 1;
    goto CLEANUP;
  }

  if ((val = malloc(sizeof(*val)*(handle->nnz))) == NULL) {
    printf("Failed to allocate 'val'.");
    rc = 1;
    goto CLEANUP;
  }

  if (wildriver_load_matrix(handle, rowptr, ind, val, NULL)) {
    printf("Successfully loaded matrix of size %lux%lu with %zu " \
        "non-zeroes.\n", handle->nrows, handle->ncols, handle->nnz);
  } else {
    printf("Failed to load matrix\n");
  }

  CLEANUP:

  if (handle != NULL) {
    wildriver_close_matrix(handle);
  }
  
  return rc; 
}

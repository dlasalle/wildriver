WildRiver is a library for reading and writing sparse matrices and graphs.

See the header file [wildriver.h](wildriver_8h.html) for details on use.


Minimal example of usage to read in a graph:

```c
#include <wildriver.h>
#include <stdio.h>

int main(
    int argc,
    char ** argv)
{
  if (argc != 2) {
    printf("Need to specify a file name\n");
    return 1;
  }

  char const * const input_file = argv[1];

  uint32_t nvtxs;
  size_t * edgePrefix;
  uint32_t * edgeList;
  double * edgeWeight; 

  rv = wildriver_read_graph(input_file, &nvtxs, NULL, NULL, NULL, \
      &edgePrefix, &edgeList, NULL, &edgeWeight);

  printf("Read graph with %u vertices and %z edges.\n", nvtxs, \
      edgePrefix[nvtxs]);

  free(edgePrefix);
  free(edgeList);
  free(edgeWeight);

  return 0;
}
```

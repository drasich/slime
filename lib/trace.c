#include "trace.h"
#include <stdio.h>
#include <stdlib.h>

void trace()
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = (char**) backtrace_symbols (array, size);

  printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++)
  printf ("%s\n", strings[i]);

  free (strings);
}

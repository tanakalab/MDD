#include <stdio.h>
#include <stdlib.h>
#include "cudd-3.0.0/cudd/cudd.h"
#include "include/DdList.h"

void free_behaviors(DdList *behaviors){

  if(behaviors-> next != NULL)
    free_behaviors(behaviors -> next);
  
  free(behaviors);
}

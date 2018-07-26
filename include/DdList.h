#ifndef _DDLIST_H_
#define _DDLIST_H_

typedef struct ddList
{
  DdNode *node;
  struct ddList *next;
} DdList;

DdList* makeDdList(DdManager*,DdList*,DdNode**,int);
void free_behaviors(DdList*);

#endif //_DDLIST_H_

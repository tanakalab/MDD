#pragma once

#include "bool.h"
#include "pair.h"
#include <string.h>

/*************** integer ***************/
struct LIST_INT_CELL {
  int key;
  struct LIST_INT_CELL* prev;
  struct LIST_INT_CELL* next;
};
typedef struct LIST_INT_CELL list_int_cell;

struct LIST_INT {
  list_int_cell* head;
  list_int_cell* last;
  unsigned size;
};
typedef struct LIST_INT list_int;

list_int* mk_new_list_int(const int);
void list_int_add_rear(list_int*, const int);
void list_int_add_rear_sub(list_int*, list_int_cell*);
void list_int_delete(list_int*, const int);
void list_int_delete_sub(list_int*, list_int_cell*);
list_int_cell* list_int_search(list_int*, const int);
list_int* list_int_copy(const list_int*);
list_int* list_int_concat(list_int*, list_int*);
bool list_int_empty(const list_int*);
void list_int_print(const list_int*);
void list_int_clear(list_int*);

list_int* mk_new_list_int(const int i) {
  list_int* L = (list_int*)calloc(1, sizeof(list_int));
  list_int_cell* neu = (list_int_cell*)calloc(1, sizeof(list_int_cell));
  ++L->size;
  neu->key = i;
  neu->prev = neu->next = NULL;
  L->head = L->last = neu;
  return L;
}

void list_int_add_rear(list_int* L, const int i) {
  if (NULL == L) {
    L = (list_int*)calloc(1, sizeof(list_int));
    L->head = L->last = NULL;
  }
  if (0 == L->size)
    L->head = L->last = NULL;
  list_int_cell* neu = (list_int_cell*)calloc(1, sizeof(list_int_cell));
  ++L->size;
  neu->key = i;
  list_int_add_rear_sub(L, neu);
}

list_int_cell* list_int_search(list_int* L, const int i) {
  list_int_cell* x = L->head;
  while (NULL != x && x->key != i)
    x = x->next;
  return x;
}

void list_int_add_rear_sub(list_int* L, list_int_cell* x) {
  if (NULL == L) {
    L = (list_int*)calloc(1, sizeof(list_int));
    L->head = NULL;
  }
  x->prev = L->last;
  if (NULL != L->last) /* L is not empty */
    L->last->next = x;
  else
    L->head = x;
  L->last = x;
  x->next = NULL;
}

void list_int_delete(list_int* L, const int i) {
  if (list_int_empty(L))
    return;
  list_int_cell* x = list_int_search(L, i);
  if (NULL != x) {
    list_int_delete_sub(L, x);
    x = NULL;
  }
}

void list_int_delete_sub(list_int* L, list_int_cell* x) {
  L->size = L->size-1;
  if (NULL != x->prev) /* x does not point the head of L */
    x->prev->next = x->next;
  else
    L->head = x->next;
  if (NULL != x->next) /* x does not point the last of L */
    x->next->prev = x->prev;
  else
    L->last = x->prev;
  free(x); x = NULL;
}


list_int* list_int_copy(const list_int* L) {
  if (NULL == L)
    return NULL;
  list_int* L_copy = (list_int*)calloc(1, sizeof(list_int));
  L_copy->head = L_copy->last = NULL;
  list_int_cell* p;
  for (p = L->head; NULL != p; p = p->next)
    list_int_add_rear(L_copy, p->key);
  return L_copy;
}

list_int* list_int_concat(list_int* S, list_int* T) {
  if (NULL == S) {
    if (NULL == T)
      return NULL;
    else
      return T;
  }
  else if (NULL == T)
    return S;

  /* S != NULL && T != NULL */
  list_int* R = list_int_copy(S);
  list_int_cell* p;
  for (p = T->head; NULL != p; p = p->next)
    list_int_add_rear(R, p->key);
  
  return R;
}

bool list_int_empty(const list_int* L) {
  if (NULL == L || 0 == L->size)
    return true;
  return false;
}

void list_int_print(const list_int* L) {
  if (NULL == L)
    return;
  list_int_cell* p = L->head;
  printf("%d", p->key);
  for (p = p->next; NULL != p; p = p->next)
    printf(", %d", p->key);
  putchar('\n');
}

void list_int_clear(list_int* L) {
  if (NULL == L)
    return;
  list_int_cell *p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q), q = NULL;
  }
  p = NULL;
}

/*************** pair of int ***************/

struct LIST_PAIR_INT_CELL {
  pair_int key;
  struct LIST_PAIR_INT_CELL* prev;
  struct LIST_PAIR_INT_CELL* next;
};
typedef struct LIST_PAIR_INT_CELL list_pair_int_cell;

struct LIST_PAIR_INT {
  list_pair_int_cell* head;
  list_pair_int_cell* last;
  unsigned size;
};
typedef struct LIST_PAIR_INT list_pair_int;

list_pair_int* mk_new_list_pair_int(const int, const int);
void list_pair_int_add_rear(list_pair_int*, const int, const int);
void list_pair_int_add_rear_sub(list_pair_int*, list_pair_int_cell*);
list_pair_int* list_pair_int_copy(list_pair_int*);
list_pair_int* list_pair_int_concat(list_pair_int*, list_pair_int*);
void list_pair_int_print(list_pair_int*);
void list_pair_int_clear(list_pair_int*);

list_pair_int* mk_new_list_pair_int(const int i, const int j) {
  list_pair_int* L = (list_pair_int*)calloc(1, sizeof(list_pair_int));
  list_pair_int_cell* neu = (list_pair_int_cell*)calloc(1, sizeof(list_pair_int_cell));
  ++L->size;
  neu->key.first = i;
  neu->key.second = j;
  neu->prev = neu->next = NULL;
  L->head = L->last = neu;
  return L;
}

void list_pair_int_add_rear(list_pair_int* L, const int i, const int j) {
  if (NULL == L) {
    L = (list_pair_int*)calloc(1, sizeof(list_pair_int));
    L->head = L->last = NULL;
  }
  if (0 == L->size)
    L->head = L->last = NULL;
  list_pair_int_cell* neu = (list_pair_int_cell*)calloc(1, sizeof(list_pair_int_cell));
  ++L->size;
  neu->key.first = i;
  neu->key.second = j;
  list_pair_int_add_rear_sub(L, neu);
}

void list_pair_int_add_rear_sub(list_pair_int* L, list_pair_int_cell* x) {
  if (NULL == L)
    L = (list_pair_int*)calloc(1, sizeof(list_pair_int));
  x->prev = L->last;
  if (NULL != L->last && 0 != L->size) /* L is not empty */
    L->last->next = x;
  else
    L->head = x;
  L->last = x;
  x->next = NULL;
}

list_pair_int* list_pair_int_copy(list_pair_int* L) {
  if (NULL == L)
    return NULL;
  list_pair_int* L_copy = (list_pair_int*)calloc(1, sizeof(list_pair_int));
  list_pair_int_cell* p;
  for (p = L->head; NULL != p; p = p->next)
    list_pair_int_add_rear(L_copy, p->key.first, p->key.second);
  return L_copy;
}

list_pair_int* list_pair_int_concat(list_pair_int* S, list_pair_int* T) {
  if (NULL == S) {
    if (NULL == T)
      return NULL;
    else
      return T;
  }
  else if (NULL == T)
    return S;

  /* S != NULL && T != NULL */
  list_pair_int* R = list_pair_int_copy(S);
  list_pair_int_cell* p;
  for (p = T->head; NULL != p; p = p->next)
    list_pair_int_add_rear(R, p->key.first, p->key.second);
  
  return R;
}

void list_pair_int_print(list_pair_int* L) {
  if (NULL == L)
    return;
  list_pair_int_cell* p = L->head;
  printf("(%d,%d)", p->key.first, p->key.second);
  for (p = p->next; NULL != p; p = p->next)
    printf(", (%d,%d)", p->key.first, p->key.second);
  putchar('\n');
}

void list_pair_int_clear(list_pair_int* L) {
  if (NULL == L)
    return;
  list_pair_int_cell *p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q), q = NULL;
  }
}


/*************** string ***************/
struct LIST_STRING_CELL {
  char* key;
  struct LIST_STRING_CELL* prev;
  struct LIST_STRING_CELL* next;
};
typedef struct LIST_STRING_CELL list_string_cell;

struct LIST_STRING {
  list_string_cell* head;
  list_string_cell* last;
  unsigned size;
};
typedef struct LIST_STRING list_string;

list_string* mk_new_list_string(char*);
void list_string_add_rear(list_string*, char*);
void list_string_add_rear_sub(list_string*, list_string_cell*);
list_string* list_string_copy(list_string*);
list_string* list_string_concat(list_string*, list_string*);
void list_string_print(list_string*);
void list_string_clear(list_string*);


list_string* mk_new_list_string(char* str) {
  list_string* L = (list_string*)calloc(1, sizeof(list_string));
  list_string_cell* neu = (list_string_cell*)calloc(1, sizeof(list_string_cell));
  ++L->size;
  neu->key = (char*)calloc(strlen(str)+1, sizeof(char)); 
  strcpy(neu->key, str);
  neu->prev = neu->next = NULL;
  L->head = L->last = neu;
  return L;
}

void list_string_add_rear(list_string* L, char* s) {
  if (NULL == L)
    L = (list_string*)calloc(1, sizeof(list_string));
  list_string_cell* neu = (list_string_cell*)calloc(1, sizeof(list_string_cell));
  ++L->size;
  neu->key = (char*)calloc(strlen(s)+1, sizeof(char));
  strcpy(neu->key, s); neu->key[strlen(s)] = '\0';
  list_string_add_rear_sub(L, neu);
}

void list_string_add_rear_sub(list_string* L, list_string_cell* x) {
  if (NULL == L)
    L = (list_string*)calloc(1, sizeof(list_string));
  x->prev = L->last;
  if (NULL != L->last) /* L is not empty */
    L->last->next = x;
  else
    L->head = x;
  L->last = x;
  x->next = NULL;
}


list_string* list_string_copy(list_string* L) {
  if (NULL == L)
    return NULL;
  list_string* L_copy = (list_string*)calloc(1, sizeof(list_string));
  list_string_cell* p;
  for (p = L->head; NULL != p; p = p->next)
    list_string_add_rear(L_copy, p->key);
  return L_copy;
}

list_string* list_string_concat(list_string* S, list_string* T) {
  if (NULL == S) {
    if (NULL == T)
      return NULL;
    else
      return T;
  }
  else if (NULL == T)
    return S;

  /* S != NULL && T != NULL */
  list_string* R = list_string_copy(S);
  list_string_cell* p;
  for (p = T->head; NULL != p; p = p->next)
    list_string_add_rear(R, p->key);
  
  return R;
}

void list_string_print(list_string* L) {
  if (NULL == L)
    return;
  list_string_cell* p;
  for (p = L->head; NULL != p; p = p->next)
    printf("%s\n", p->key);
}

void list_string_clear(list_string* L) {
  if (NULL == L)
    return;
  list_string_cell *p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q->key), q->key = NULL;
    free(q), q = NULL;
  }
}

/* #endif */

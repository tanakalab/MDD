/* #ifndef __DD_PRBT_HEADER_H__ */
/* #define __DD_PRBT_HEADER_H__ */

#pragma once

#include <math.h>

#define CLASSBENCH_HEADER_LENGTH 104
#define ADDRESS_LENGTH 32
#define PORT_NUMBER_LENGTH 16
#define PROTOCOL_LENGTH 8

struct HEADER {
  long num; /* header number */
  char* string;
};
typedef struct HEADER header;

char* decimal_to_binary_long(const unsigned, const long);
header* mk_header(const long, const long, const long, const long, const long, const long);
void header_print(header*);
void header_print_with_digit(header*, const unsigned);
void header_clear(header*);

/* Using codes on the following URL
   https://www.programmingsimplified.com/c/source-code/c-program-convert-decimal-to-binary
 */
char* decimal_to_binary_long(const unsigned d, const long n) {
  char* bin = (char*)malloc(d+1);
  bin[d] = '\0';
  if (NULL == bin)
    exit(EXIT_FAILURE);
  int i, c;
  long digit;
  for (i = 0, c = d-1; c >= 0; --c, ++i) {
    digit = n >> c;
    if (digit & 1)
      bin[i] = '1';
    else
      bin[i] = '0';
  }
  return bin;
}

header* mk_header(const long i, const long SA, const long DA, const long SP, const long DP, const long PROT) {
  header* neu = (header*)calloc(1, sizeof(header));
  neu->num = i;
  neu->string = (char*)calloc(CLASSBENCH_HEADER_LENGTH+1, sizeof(char));

  char* sa = decimal_to_binary_long(32, SA);
  char* da = decimal_to_binary_long(32, DA);
  char* sp = decimal_to_binary_long(16, SP);
  char* dp = decimal_to_binary_long(16, DP);
  char* prot = decimal_to_binary_long(8, PROT);
  unsigned j;
  for (j = 0; j < ADDRESS_LENGTH; ++j) {
    neu->string[j] = sa[j];
    neu->string[j+ADDRESS_LENGTH] = sa[j];
  }
  for (j = 0; j < PORT_NUMBER_LENGTH; ++j) {
    neu->string[j+ADDRESS_LENGTH*2] = sp[j];
    neu->string[j+ADDRESS_LENGTH*2+PORT_NUMBER_LENGTH] = dp[j];
  }
  for (j = 0; j < PROTOCOL_LENGTH; ++j)
    neu->string[j+ADDRESS_LENGTH*2+PORT_NUMBER_LENGTH*2] = prot[j];
  neu->string[CLASSBENCH_HEADER_LENGTH] = '\0';
  free(sa); free(da); free(sp); free(dp); free(prot);
  sa = da = sp = dp = prot = NULL;
  return neu;
}

void header_print(header* h) {
  printf("h[%ld] = %s", h->num, h->string);
}

void header_print_with_digit(header* h, const unsigned d) {
  printf("h[%*ld] = %s", d, h->num, h->string);
}

void header_clear(header* h) {
  free(h->string), h->string = NULL;
  free(h);
}

struct LIST_HEADER_CELL {
  header* key;
  struct LIST_HEADER_CELL* prev;
  struct LIST_HEADER_CELL* next;
};
typedef struct LIST_HEADER_CELL list_header_cell;

struct LIST_HEADER {
  list_header_cell* head;
  list_header_cell* last;
  unsigned size;
};
typedef struct LIST_HEADER list_header;

list_header* read_list_of_class_bench_headers(char*);
void list_header_add_rear_sub(list_header*, list_header_cell*);
void list_header_add_rear_with_raw_data(list_header*, const long, const long, const long, const long, const long, const long);
void list_header_print(list_header*);
void list_header_clear(list_header*);


list_header* read_list_of_class_bench_headers(char* header_file_name) {
  FILE* fp;
  if (NULL == (fp = fopen(header_file_name, "r"))) {
    fprintf(stderr, "ERROR: Cna't read the header list file.\n");
    exit(1);
  }
  
  list_header* H = (list_header*)calloc(1, sizeof(list_header));
  long SA, DA, SP, DP, PROT, FLAG;
  unsigned i;
  // Ignore the FLAG field
  for (i = 0; EOF != fscanf(fp, "%ld %ld %ld %ld %ld %ld", &SA, &DA, &SP, &DP, &PROT, &FLAG); ++i)
    list_header_add_rear_with_raw_data(H, i, SA, DA, SP, DP, PROT);
  return H;
}

void list_header_add_rear_sub(list_header* H, list_header_cell* x) {
  x->prev = H->last;
  if (NULL != H->last)
    H->last->next = x;
  else
    H->head = x;
  H->last = x;
  x->next = NULL;
}

void list_header_add_rear_with_raw_data(list_header* H, const long i, const long SA, const long DA, const long SP, const long DP, const long PROT) {
  list_header_cell* neu = (list_header_cell*)calloc(1, sizeof(list_header_cell));
  ++H->size;
  neu->key = mk_header(i, SA, DA, SP, DP, PROT);
  list_header_add_rear_sub(H, neu);
}

void list_header_print(list_header* H) {
  if (NULL == H)
    return;
  const unsigned d = (unsigned)floor(log10(H->size-1))+1;
  list_header_cell* h;
  for (h = H->head; NULL != h; h = h->next)
    header_print_with_digit(h->key, d), putchar('\n');
}

void list_header_clear(list_header* H) {
  if (NULL == H)
    return;
  list_header_cell *p, *q;
  for (p = H->head; NULL != p; ) {
    q = p;
    p = p->next;
    header_clear(q->key), q->key = NULL;
    free(q), q = NULL;
  }
}

/* #endif */

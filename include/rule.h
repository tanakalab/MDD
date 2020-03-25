#pragma once

#include "bool.h"
#include "header.h"
#include <math.h>

#define ADDRESS_LENGTH 32
#define PORT_LENGTH 16
#define PROTOCOL_LENGTH 8

#define DEFAULTADDRESS "********************************"
#define DEFAULTPORTNUMBER "****************"
#define DEFAULTPORTNUMBER_L 0
#define DEFAULTPORTNUMBER_R 65535
#define DEFAULTPROTOCOLNUMBER "********"

#define SA_START_POINT 0
#define DA_START_POINT 32
#define SP_START_POINT 64
#define DP_START_POINT 80
#define PROT_START_POINT 96

unsigned* NUMBER_OF_RUNS; // 各ルールを構成する連の数を格納

struct RULE {
  unsigned num; /* rule number */
  char* sa; /* source address */
  char* da; /* destination address */
  unsigned spl; /* left limit point of source port interval */
  unsigned spr; /* right limit point of source port interval */
  unsigned dpl; /* left limit point of destination port interval */
  unsigned dpr; /* right limit point of destination port interval */
  char* prot; /* protocol number */
};
typedef struct RULE rule;

char* decimal_to_binary(const unsigned, const int);
char* set_address(const int, const int, const int, const int, const int);
char* set_protocol(const int, const int);
rule* mk_rule(const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int);
/* rule* rule_copy(rule*); */
void rule_print(rule*);
void rule_print_with_digit(rule*, const unsigned);
void rule_clear(rule*);



/* Using codes on the following URL
   https://www.programmingsimplified.com/c/source-code/c-program-convert-decimal-to-binary
 */
char* decimal_to_binary(const unsigned d, const int n) {
  char* bin = (char*)malloc(d+1);
  bin[d] = '\0';
  if (NULL == bin)
    exit(EXIT_FAILURE);
  int digit, i, c;
  for (i = 0, c = d-1; c >= 0; --c, ++i) {
    digit = n >> c;
    if (digit & 1)
      bin[i] = '1';
    else
      bin[i] = '0';
  }
  return bin;
}

char* set_address(const int o1, const int o2, const int o3, const int o4, const int prefix) {
  char* add = (char*)calloc(ADDRESS_LENGTH+1, sizeof(char));
  char* bino1 = decimal_to_binary(8, o1);
  char* bino2 = decimal_to_binary(8, o2);
  char* bino3 = decimal_to_binary(8, o3);
  char* bino4 = decimal_to_binary(8, o4);
  unsigned i;
  for (i = 0; i < 8; ++i) {
    add[i]    = bino1[i];
    add[i+8]  = bino2[i];
    add[i+16] = bino3[i];
    add[i+24] = bino4[i];
  }
  for (i = prefix; i < ADDRESS_LENGTH; ++i)
    add[i] = '*';
  add[ADDRESS_LENGTH] = '\0';
  free(bino1), free(bino2), free(bino3), free(bino4);
  bino1 = bino2 = bino3 = bino4 = NULL;
  return add;
}

char* set_protocol(const int p, const int pm) {
  /* printf("pm = %d\n", pm); */
  int pp = p & pm;
  char* protocol = decimal_to_binary(8, pp);
  /* for (i = pm; i < PROTOCOL_LENGTH; ++i) */
  /*   protocol[i] = '*'; */
  protocol[PROTOCOL_LENGTH] = '\0';
  /* printf("protocol = %s\n", protocol); */
  return protocol;
}

rule* mk_rule(const int number, const int sa1, const int sa2, const int sa3, const int sa4, const int sap, const int da1, const int da2, const int da3, const int da4, const int dap, const int spl, const int spr, const int dpl, const int dpr, const int protocol, const int protocolm) {
  rule* neu = (rule*)calloc(1, sizeof(rule));
  neu->num = number;
  neu->spl = spl;
  neu->spr = spr;
  neu->dpl = dpl;
  neu->dpr = dpr;
  neu->sa = set_address(sa1, sa2, sa3, sa4, sap);
  neu->da = set_address(da1, da2, da3, da4, dap);
  neu->prot = set_protocol(protocol, protocolm);
  return neu;
}

/* rule* rule_copy(rule* r) { */
/*   if (NULL == r) */
/*     return NULL; */
/*   rule* copy = (rule*)calloc(1, sizeof(rule)); */
/*   copy->num = r->num; */
/*   copy->spl = r->spl; */
/*   copy->spr = r->spr; */
/*   copy->dpl = r->dpl; */
/*   copy->dpr = r->dpr; */
/*   copy->sa = (char*)calloc(ADDRESS_LENGTH+1, sizeof(char)); strcpy(copy->sa, r->sa); copy->sa[ADDRESS_LENGTH] = '\0'; */
/*   copy->da = (char*)calloc(ADDRESS_LENGTH+1, sizeof(char)); strcpy(copy->da, r->da); copy->da[ADDRESS_LENGTH] = '\0'; */
/*   copy->prot = (char*)calloc(PROTOCOL_LENGTH+1, sizeof(char)); strcpy(copy->prot, r->prot); copy->prot[PROTOCOL_LENGTH] = '\0'; */
/*   return copy; */
/* }   */

void rule_print(rule* r) {
  printf("r[%u] = %s %s %u : %u %u : %u %s", r->num, r->sa, r->da, r->spl, r->spr, r->dpl, r->dpr, r->prot);
}

void rule_print_with_digit(rule* r, const unsigned d) {
  printf("r[%*u] = %s %s %u : %u %u : %u %s", d, r->num, r->sa, r->da, r->spl, r->spr, r->dpl, r->dpr, r->prot);
}

void rule_clear(rule* r) {
  free(r->sa), r->sa = NULL;
  free(r->da), r->da = NULL;
  free(r->prot), r->prot = NULL;
  free(r);
}

struct LIST_RULE_CELL {
  rule* key;
  struct LIST_RULE_CELL* prev;
  struct LIST_RULE_CELL* next;
};
typedef struct LIST_RULE_CELL list_rule_cell;

struct LIST_RULE {
  list_rule_cell* head;
  list_rule_cell* last;
  unsigned size;
};
typedef struct LIST_RULE list_rule;

list_rule* read_list_of_class_bench_rules(char*);
void list_rule_add_rear_sub(list_rule*, list_rule_cell*);
/* void list_rule_add_rear(list_rule*, rule*); */
void list_rule_add_rear_with_raw_data(list_rule*, const unsigned, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int);
void count_the_number_of_runs(list_rule*);
unsigned count_the_number_of_runs_sub(rule*);
void list_rule_print(list_rule*);
void list_rule_clear(list_rule*);
  
/* ignore the flag field (final field) */
list_rule* read_list_of_class_bench_rules(char* rule_file_name) {
  FILE* fp;
  if (NULL == (fp = fopen(rule_file_name, "r"))) {
    fprintf(stderr, "ERROR: Can't open the rule list file.\n");
    exit(1);
  }

  list_rule* R = (list_rule*)calloc(1, sizeof(list_rule));
  int SA1, SA2, SA3, SA4, SAPrefix, DA1, DA2, DA3, DA4, DAPrefix;
  int SPL, SPR, DPL, DPR, PROT, PROTMask;
  char FLAG[255];
  unsigned i;
  for (i = 0; EOF != fscanf(fp,"@%d.%d.%d.%d/%d %d.%d.%d.%d/%d %d : %d %d : %d %x/%x %s\n", &SA1, &SA2, &SA3, &SA4, &SAPrefix, &DA1, &DA2, &DA3, &DA4, &DAPrefix, &SPL, &SPR, &DPL, &DPR, &PROT, &PROTMask, FLAG); ++i)
    list_rule_add_rear_with_raw_data(R, i, SA1, SA2, SA3, SA4, SAPrefix, DA1, DA2, DA3, DA4, DAPrefix, SPL, SPR, DPL, DPR, PROT, PROTMask);
  
  return R;
}

void list_rule_add_rear_sub(list_rule* R, list_rule_cell* x) {
  x->prev = R->last;
  if (NULL != R->last)
    R->last->next = x;
  else
    R->head = x;
  R->last = x;
  x->next = NULL;
}

/* void list_rule_add_rear(list_rule* R, rule* r) { */
/*   list_rule_cell* new = (list_rule_cell*)calloc(1, sizeof(list_rule_cell)); */
/*   ++R->size; */
/*   new->key = rule_copy(r); */
/*   list_rule_add_rear_sub(R, new); */
/* } */

void list_rule_add_rear_with_raw_data(list_rule* R, const unsigned i, const int sa1, const int sa2, const int sa3, const int sa4, const int sap, const int da1, const int da2, const int da3, const int da4, const int dap, const int spl, const int spr, const int dpl, const int dpr, const int protocol, const int protocolm) {
  list_rule_cell* neu = (list_rule_cell*)calloc(1, sizeof(list_rule_cell));
  ++R->size;
  neu->key = mk_rule(i, sa1, sa2, sa3, sa4, sap, da1, da2, da3, da4, dap, spl, spr, dpl, dpr, protocol, protocolm);
  list_rule_add_rear_sub(R, neu);
}

void count_the_number_of_runs(list_rule* R) {
  NUMBER_OF_RUNS = (unsigned*)calloc(R->size, sizeof(unsigned));
  list_rule_cell* r;
  for (r = R->head; NULL != r; r = r->next)
    NUMBER_OF_RUNS[r->key->num] = count_the_number_of_runs_sub(r->key);
}

unsigned count_the_number_of_runs_sub(rule* r) {
  unsigned sum = 0;
  if (strcmp(DEFAULTADDRESS, r->sa))
    ++sum;
  if (strcmp(DEFAULTADDRESS, r->da))
    ++sum;
  if (DEFAULTPORTNUMBER_L != r->spl || DEFAULTPORTNUMBER_R != r->spr)
    ++sum;
  if (DEFAULTPORTNUMBER_L != r->dpl || DEFAULTPORTNUMBER_R != r->dpr)
    ++sum;
  if (strcmp(DEFAULTPROTOCOLNUMBER, r->prot))
    ++sum;
  return sum;
}

void list_rule_print(list_rule* R) {
  if (NULL == R)
    return;
  const unsigned d = (unsigned)floor(log10(R->size-1))+1;
  list_rule_cell* p;
  for (p = R->head; NULL != p; p = p->next)
    rule_print_with_digit(p->key, d), putchar('\n');
}

void list_rule_clear(list_rule* R) {
  if (NULL == R)
    return;
  list_rule_cell *p, *q;
  for (p = R->head; NULL != p; ) {
    q = p;
    p = p->next;
    rule_clear(q->key), q->key = NULL;
    free(q), q = NULL;
  }
}

bool does_match(rule*, header*);
unsigned linear_search(list_rule*, header*);
void do_linear_search(list_rule*, list_header*);


bool does_match(rule* r, header* h) {
  unsigned i;
  for (i = 0; i < ADDRESS_LENGTH; ++i) {
    if ('*' != r->sa[i] && h->string[i] != r->sa[i])
      return false;
    if ('*' != r->da[i] && h->string[ADDRESS_LENGTH+i] != r->da[i])
      return false;
  }
  for (i = 0; i < PROTOCOL_LENGTH; ++i)
    if ('*' != r->prot[i] && h->string[ADDRESS_LENGTH*2+PORT_NUMBER_LENGTH*2+i] != r->prot[i])
      return false;
  
  char SP[PORT_NUMBER_LENGTH+1], DP[PORT_NUMBER_LENGTH+1];
  for (i = 0; i < PORT_NUMBER_LENGTH; ++i) {
    SP[i] = h->string[ADDRESS_LENGTH*2+i];
    DP[i] = h->string[ADDRESS_LENGTH*2+PORT_NUMBER_LENGTH+i];
  }
  SP[PORT_NUMBER_LENGTH] = DP[PORT_NUMBER_LENGTH] = '\0';

  unsigned sp = (unsigned)strtol(SP, NULL, 2);
  unsigned dp = (unsigned)strtol(DP, NULL, 2);

  if (!(r->spl <= sp && sp <= r->spr))
    return false;
  if (!(r->dpl <= dp && dp <= r->dpr))
    return false;
  
  return true;
}

unsigned linear_search(list_rule* R, header* h) {
  list_rule_cell* r;
  for (r = R->head; NULL != r; r = r->next)
    if (does_match(r->key, h))
      return r->key->num;
  return R->size;
}

void do_linear_search(list_rule* R, list_header* H) {
  printf("==================== Linear Search ====================\n");
  list_header_cell* h;
  for (h = H->head; NULL != h; h = h->next)
    printf("h[%ld] = %s ---> %d\n", h->key->num, h->key->string, linear_search(R, h->key));
}

/* #endif */

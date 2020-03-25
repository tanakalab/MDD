#pragma once

#include "list.h"
#include "rule.h"
#include <inttypes.h>

#define PROT_REVERSE_START_POINT 0
#define DP_REVERSE_START_POINT 8
#define SP_REVERSE_START_POINT 24
#define DA_REVERSE_START_POINT 40
#define SA_REVERSE_START_POINT 72

struct RUN {
  char* string;
  unsigned rule_num;
  unsigned run_num;
};
typedef struct RUN run;

run* mk_run(char*, const unsigned, const unsigned);
run* run_copy(run*);
void run_print(run*);
void run_clear(run*);

run* mk_run(char* s, const unsigned rule_number, const unsigned run_number) {
  run* r = (run*)calloc(1, sizeof(run));
  r->string = (char*)calloc(strlen(s)+1, sizeof(char));
  strcpy(r->string, s); r->string[strlen(s)] = '\0';
  r->rule_num = rule_number;
  r->run_num = run_number;
  return r;
}

run* run_copy(run* r) {
  if (NULL == r)
    return NULL;
  run* copy = (run*)calloc(1, sizeof(run));
  copy->rule_num = r->rule_num;
  copy->run_num = r->run_num;
  copy->string = (char*)calloc(strlen(r->string)+1, sizeof(char));
  strcpy(copy->string, r->string);
  copy->string[strlen(r->string)] = '\0';
  return copy;
}

void run_print(run* r) {
  if (NULL == r)
    return;
  printf("r[%u,%u] = %s", r->rule_num, r->run_num, r->string);
}

void run_clear(run* r) {
  if (NULL == r)
    return;
  free(r->string), r->string = NULL;
  free(r);
}

struct LIST_RUN_CELL {
  run* key;
  struct LIST_RUN_CELL* prev;
  struct LIST_RUN_CELL* next;
};
typedef struct LIST_RUN_CELL list_run_cell;

struct LIST_RUN {
  list_run_cell* head;
  list_run_cell* last;
  unsigned size;
};
typedef struct LIST_RUN list_run;

void list_run_add_rear_sub(list_run*, list_run_cell*);
void list_run_add_rear(list_run*, run*);
void list_run_add_rear_with_raw_data(list_run*, char*, const unsigned, const unsigned);
list_run* list_run_copy(const list_run*);
void list_run_concat(list_run*, const list_run*);
void list_run_print(list_run*);
void list_run_clear(list_run*);

void list_run_add_rear(list_run* runs, run* r) {
  if (NULL == runs)
    runs = (list_run*)calloc(1, sizeof(list_run));
  list_run_cell* neu = (list_run_cell*)calloc(1, sizeof(list_run_cell));
  ++runs->size;
  neu->key = run_copy(r);
  list_run_add_rear_sub(runs, neu);
}

void list_run_add_rear_sub(list_run* runs, list_run_cell* x) {
  if (NULL == runs)
    runs = (list_run*)calloc(1, sizeof(list_run));
  x->prev = runs->last;
  if (NULL != runs->last)
    runs->last->next = x;
  else
    runs->head = x;
  runs->last = x;
  x->next = NULL;
}

void list_run_add_rear_with_raw_data(list_run* runs, char* str, const unsigned rule_num, const unsigned run_num) {
  list_run_cell* neu = (list_run_cell*)calloc(1, sizeof(list_run_cell));
  ++runs->size;
  neu->key = mk_run(str, rule_num, run_num);
  list_run_add_rear_sub(runs, neu);
}

list_run* list_run_copy(const list_run* L) {
  if (NULL == L)
    return NULL;
  list_run* L_copy = (list_run*)calloc(1, sizeof(list_run));
  list_run_cell* p;
  for (p = L->head; NULL != p; p = p->next)
    list_run_add_rear(L_copy, p->key);
  return L_copy;
}

/* L1 and L2 do not point to NULL */
void list_run_concat2(list_run* L1, const list_run* L2) {
  list_run_cell* p;
  for (p = L2->head; NULL != p; p = p->next)
    list_run_add_rear(L1, p->key);
}

void list_run_print(list_run* r) {
  if (NULL == r)
    return;
  list_run_cell* p;
  for (p = r->head; NULL != p; p = p->next)
    run_print(p->key), putchar('\n');
}

void list_run_clear(list_run* r) {
  if (NULL == r)
    return;
  list_run_cell *p, *q;
  for (p = r->head; NULL != p; ) {
    q = p;
    p = p->next;
    run_clear(q->key), q->key = NULL;
    free(q), q = NULL;
  }
}

struct RBT {
  int var; /* variable number */
  unsigned trie_number; /* for clear node in rbt with pointers */
  list_run* runs;
  long id; /* for making and searchng decision diagram */
  char* reachable; /* for making decision diagram */
  struct RBT* left;
  struct RBT* right;
};
typedef struct RBT rbt;

/* uint16_t NUMBER_OF_NODES_WITH_RUN; */
unsigned long NUMBER_OF_NODES_WITH_RUN;

rbt** make_run_based_trie(list_rule*);
void make_root_nodes(rbt**);
rbt* make_rbt_node(const int, const unsigned);
void make_run_based_trie_nodes_for_rule(rbt**, rule*);
run* cut_run_from_address_field(rule*, char*, unsigned*);
unsigned get_end_point_of_bit_sequence(char*);
list_run* cut_runs_from_port_field(rule*, unsigned, unsigned, unsigned*);
list_string* range_to_01ms(const unsigned, const unsigned, const unsigned, const unsigned);
list_string* unsigned_pair_to_01m(const unsigned, const unsigned, const unsigned);
void cut_mask(list_string*);
bool contain_mask(char*);
void cut_mask_sub(char*);
run* cut_run_from_protocol_field(rule*, char*, unsigned*);
void traverse_and_make_rbt_nodes_by_run(rbt*, const unsigned, run*);
void add_id_to_rbt_node(rbt*);
void add_run_to_rbt_node(rbt*, run*);
void traverse_and_make_rbt_nodes_by_runs(rbt*, const unsigned, list_run*);

void run_based_trie_print(rbt*, const char);
void run_based_tries_print(rbt**);
bool has_only_root_node(rbt*);
void run_based_trie_clear(rbt*, const unsigned);
void run_based_tries_clear(rbt**);

rbt** make_run_based_trie(list_rule* R) {
  rbt** T = (rbt**)calloc(CLASSBENCH_HEADER_LENGTH, sizeof(rbt*));
  make_root_nodes(T);
  list_rule_cell* p;
  NUMBER_OF_NODES_WITH_RUN = 0;
  for (p = R->head; NULL != p; p = p->next)
    make_run_based_trie_nodes_for_rule(T, p->key);
  return T;
}

void make_root_nodes(rbt** T) {
  unsigned i;
  for (i = 0; i < CLASSBENCH_HEADER_LENGTH; ++i)
    T[i] = make_rbt_node(-1, i);
}

rbt* make_rbt_node(const int v, const unsigned t_num) {
  rbt* node = (rbt*)calloc(1, sizeof(rbt));
  node->var = v;
  node->trie_number = t_num;
  node->runs = NULL;
  node->id = -1;
  node->left = node->right = NULL;
  return node;
}

void make_run_based_trie_nodes_for_rule(rbt** T, rule* r) {
  unsigned run_num_count = 0;
  
  run* sa = cut_run_from_address_field(r, r->sa, &run_num_count);
  run* da = cut_run_from_address_field(r, r->da, &run_num_count);
  list_run* sp = cut_runs_from_port_field(r, r->spl, r->spr, &run_num_count);
  list_run* dp = cut_runs_from_port_field(r, r->dpl, r->dpr, &run_num_count);
  run* prot = cut_run_from_protocol_field(r, r->prot, &run_num_count);

  /* traverse_and_make_rbt_nodes_by_run(T[SA_REVERSE_START_POINT], SA_REVERSE_START_POINT, sa); */
  /* traverse_and_make_rbt_nodes_by_run(T[DA_REVERSE_START_POINT], DA_REVERSE_START_POINT, da); */
  /* traverse_and_make_rbt_nodes_by_runs(T[SP_REVERSE_START_POINT], SP_REVERSE_START_POINT, sp); */
  /* traverse_and_make_rbt_nodes_by_runs(T[DP_REVERSE_START_POINT], DP_REVERSE_START_POINT, dp); */
  /* traverse_and_make_rbt_nodes_by_run(T[PROT_REVERSE_START_POINT], PROT_REVERSE_START_POINT, prot); */

  traverse_and_make_rbt_nodes_by_run(T[SA_START_POINT], SA_START_POINT, sa);
  traverse_and_make_rbt_nodes_by_run(T[DA_START_POINT], DA_START_POINT, da);
  traverse_and_make_rbt_nodes_by_runs(T[SP_START_POINT], SP_START_POINT, sp);
  traverse_and_make_rbt_nodes_by_runs(T[DP_START_POINT], DP_START_POINT, dp);
  traverse_and_make_rbt_nodes_by_run(T[PROT_START_POINT], PROT_START_POINT, prot);

  run_clear(sa);
  run_clear(da);
  list_run_clear(sp);
  list_run_clear(dp);
  run_clear(prot);
}

run* cut_run_from_address_field(rule* r, char* add, unsigned* run_num_count) {
  unsigned ep = get_end_point_of_bit_sequence(add);
  /* printf("ep = %u\n", ep); */
  if (0 == ep)
    return NULL;

  run* neu = (run*)calloc(1, sizeof(run));
  neu->string = (char*)calloc(ep+1, sizeof(char));
  strncpy(neu->string, add, ep);
  neu->string[ep] = '\0';
  neu->rule_num = r->num;
  neu->run_num = ++(*run_num_count);
  /* printf("string = %s, rule number = %u, run number = %u\n", neu->string, neu->rule_num, neu->run_num); */
  return neu;
}

unsigned get_end_point_of_bit_sequence(char* str) {
  const unsigned l = strlen(str);
  unsigned i;
  for (i = 0; i < l; ++i)
    if ('*' == str[i])
      break;
  return i;
}

list_run* cut_runs_from_port_field(rule* rule, unsigned left, unsigned right, unsigned* run_num_count) {
  if (left == DEFAULTPORTNUMBER_L && right == DEFAULTPORTNUMBER_R)
    return NULL;
  list_string* RUN = range_to_01ms(left, right, DEFAULTPORTNUMBER_L, DEFAULTPORTNUMBER_R);
  cut_mask(RUN);
  /* list_string_print(RUN); */

  ++(*run_num_count);
  list_run* runs = (list_run*)calloc(1, sizeof(list_run));
  list_string_cell* p;
  for (p = RUN->head; NULL != p; p = p->next)
    list_run_add_rear_with_raw_data(runs, p->key, rule->num, *run_num_count);
  
  return runs;
}

list_string* range_to_01ms(const unsigned a, const unsigned b, const unsigned low, const unsigned high) {
  unsigned m = (low+high-1)/2;
  unsigned n = (low+high+1)/2;

  if (a == low && b == high)
    return unsigned_pair_to_01m(a, b, PORT_LENGTH);
  else if (b <= m)
    return range_to_01ms(a, b, low, m);
  else if (a <= m && n <= b) {
    list_string* S = range_to_01ms(a, m, low, m);
    list_string* T = range_to_01ms(n, b, n, high);
    list_string* R = list_string_concat(S, T);
    list_string_clear(S); S = NULL;
    list_string_clear(T); T = NULL;
    return R;
  }
  else
    return range_to_01ms(a, b, n, high);

  return NULL;
}

list_string* unsigned_pair_to_01m(const unsigned a, const unsigned b, const unsigned w) {
  char* bin = (char*)calloc(w+1, sizeof(char));

  double a1 = a, b1 = b, l;
  int d;
  l = log2(b1-a1+1);
  d = (int)l;

  int i;
  for (i = w-1; i >= 0; --i)
    bin[w-i-1] = 48 + ((a>>i)&1);
  bin[w] = '\0';

  for (i = 0; i < d; ++i)
    bin[w-i-1] = '*';

  return mk_new_list_string(bin);
}

void cut_mask(list_string* RUN) {
  list_string_cell* p;
  for (p = RUN->head; NULL != p; p = p->next)
    if (contain_mask(p->key))
      cut_mask_sub(p->key);
}

bool contain_mask(char* s) {
  /* printf("l = %u\n", strlen(s)); */
  unsigned i;
  for (i = 0; i < strlen(s); ++i)
    if ('*' == s[i])
      return true;
  return false;
}

void cut_mask_sub(char* s) {
  unsigned l = strlen(s), i;
  for (i = 0; i < l; ++i)
    if ('*' == s[i])
      break;
  s[i] = '\0';
}

run* cut_run_from_protocol_field(rule* r, char* prot, unsigned* run_num_count) {
  unsigned ep = get_end_point_of_bit_sequence(prot);
  /* printf("ep = %u\n", ep); */
  if (0 == ep)
    return NULL;

  run* neu = (run*)calloc(1, sizeof(run));

  neu->string = (char*)calloc(ep+1, sizeof(char));
  strncpy(neu->string, prot, ep);
  neu->string[ep] = '\0';
  neu->rule_num = r->num;
  neu->run_num = ++(*run_num_count);
  /* printf("string = %s, rule number = %u, run number = %u\n", neu->string, neu->rule_num, neu->run_num); */
  return neu;
}

void traverse_and_make_rbt_nodes_by_run(rbt* T, const unsigned tn, run* r) {
  if (NULL == r)
    return;
  rbt* ptr = T;
  const unsigned l = strlen(r->string);
  unsigned i;
  for (i = 0; i < l; ++i) {
    if ('0' == r->string[i]) {
      if (NULL == ptr->left)
	ptr->left = make_rbt_node(tn+1+i, tn);
      ptr = ptr->left;
    }
    else {
      if (NULL == ptr->right)
	ptr->right = make_rbt_node(tn+1+i, tn);
      ptr = ptr->right;
    }
  }
  add_id_to_rbt_node(ptr);
  add_run_to_rbt_node(ptr, r);
}

void add_id_to_rbt_node(rbt* ptr) {
  if (-1 == ptr->id)
    ptr->id = NUMBER_OF_NODES_WITH_RUN++;
}

void add_run_to_rbt_node(rbt* ptr, run* r) {
  if (NULL == ptr->runs)
    ptr->runs = (list_run*)calloc(1, sizeof(list_run));
  list_run_add_rear(ptr->runs, r);
  /* list_run_print(ptr->runs); */
}

void traverse_and_make_rbt_nodes_by_runs(rbt* T, const unsigned tn, list_run* runs) {
  if (NULL == runs)
    return;
  list_run_cell* p;
  for (p = runs->head; NULL != p; p = p->next)
    traverse_and_make_rbt_nodes_by_run(T, tn, p->key);
}

void run_based_trie_print(rbt* ptr, const char b) {
  if (NULL == ptr)
    return;

  run_based_trie_print(ptr->left, '0');
  run_based_trie_print(ptr->right, '1');

  printf("%c [v = %d] [tn = %u] {", b, ptr->var, ptr->trie_number);
  if (NULL != ptr->runs) {
    /* printf(" [run_number = %d]\n", ptr->run_number); */
    putchar('\n');
    list_run_print(ptr->runs);
    printf("}\n");
  }
  else
    printf(" }\n");
}

void run_based_tries_print(rbt** T) {
  unsigned i;
  for (i = 0; i < CLASSBENCH_HEADER_LENGTH; ++i) {
    if (!has_only_root_node(T[i])) {
      printf("========== T[%u] ==========\n", i);
      run_based_trie_print(T[i], '-');
    }
  }
}

/* passed argument is root node */
bool has_only_root_node(rbt* T) {
  if (NULL == T)
    return false;
  if (NULL == T->left && NULL == T->right)
    return true;
  return false;
}

void run_based_trie_clear(rbt* ptr, const unsigned tn) {
  if (NULL == ptr || tn != ptr->trie_number)
    return;

  run_based_trie_clear(ptr->left, tn);
  run_based_trie_clear(ptr->right, tn);

  list_run_clear(ptr->runs), ptr->runs = NULL;
  if (NULL != ptr->reachable)
    free(ptr->reachable), ptr->reachable = NULL;
  
  if (NULL != ptr->left && tn == ptr->left->trie_number)
    free(ptr->left), ptr->left = NULL;
  if (NULL != ptr->right && tn == ptr->right->trie_number)
    free(ptr->right), ptr->right = NULL;
}

void run_based_tries_clear(rbt** T) {
  unsigned i;
  for (i = 0; i < CLASSBENCH_HEADER_LENGTH; ++i)
    run_based_trie_clear(T[i], i), T[i] = NULL;
}

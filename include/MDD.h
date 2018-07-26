#ifndef _MDD_H_
#define _MDD_H_

typedef struct mddNode
{
  int k; // continuous k bits to aggregate
  unsigned int index; // variable index
  int value; // value of node if terminal; otherwise -1
  DdNode *ddnode; // mtbdd node for MddNode
  struct mddNode **next;
  
} MddNode;

void keepMemoryForHash(unsigned int,DdList*,int);
unsigned int hashFunction(DdNode*);
int doesNotExist(DdNode*);
//int* intToArray(int,int);
void set(MddNode*);
MddNode* get(DdNode*);
MddNode* makeMDD(int,DdManager*,DdNode*);
MddNode* MDD_Eval(int,int,MddNode*,int*);

#endif //_MDD_H_

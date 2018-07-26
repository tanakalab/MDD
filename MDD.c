#include <stdio.h>
#include <stdlib.h>
#include "cudd-3.0.0/cudd/cudd.h" 
#include "include/DdList.h"
#include "include/MDD.h"

static MddNode **hash;
static unsigned int hashSize;

void keepMemoryForHash(unsigned int HS,DdList* terminalList,int k){

  hashSize = HS;

  if((hash = (MddNode**)malloc(sizeof(MddNode*)*hashSize)) == NULL){
    printf("memory for hash error!!\n");
    exit(EXIT_FAILURE);
  }
  int i;
  for(i=0;i<hashSize;i++)
    hash[i]=NULL;
  
  while(terminalList != NULL){
    MddNode *mddNode;
    if(( mddNode = (MddNode*)malloc(sizeof(MddNode))) == NULL){
      printf("memory for mddNode error!!\n");
      exit(EXIT_FAILURE);
    }
    mddNode -> k = k;
    mddNode -> index = Cudd_NodeReadIndex(terminalList->node);
    mddNode -> value = (int)Cudd_V(terminalList->node);
    mddNode -> next = NULL;
    mddNode -> ddnode = terminalList->node;
    hash[(int)Cudd_V(terminalList->node)] = mddNode;
    
    terminalList = terminalList -> next;
  }

}

unsigned int hashFunction(DdNode *mtbdd){

  if(Cudd_IsConstant(mtbdd))
    return Cudd_V(mtbdd);
 
  uintptr_t value = (uintptr_t)mtbdd;

  //srand(seed);
  /* int i=0,sum=0; */
  /* char array[256]; */

  /* sprintf(array,"%p",mtbdd); */
  /* while(array[i] != '\0'){ */
  /*   sum+=array[i]; */
  /*   i++; */
  /* } */
  
  /* return sum % hashSize; */
  return value % hashSize;
}

int doesNotExist(DdNode *mtbdd){

  if(hash[hashFunction(mtbdd)] == NULL)
    return 1;

  int i;
  MddNode *MN;
  for(i = 0; i < hashSize; i++){
    if( (MN = hash[(hashFunction(mtbdd)+i) % hashSize]) != NULL && MN -> ddnode == mtbdd)
      return 0;
  }

  return 1;

}

void set(MddNode *mdd){
  
  int i=0,hashValue;
  while( hash[ hashValue=((hashFunction(mdd->ddnode)+i) % hashSize)] != NULL )
    i++;
  
  hash[hashValue] = mdd;
  //hash[(hashFunction(mdd->ddnode)+i) % hashSize] = mdd;
}

MddNode* get(DdNode *mtbdd){

  if(hash[hashFunction(mtbdd)] == NULL){
      printf("impossible error in get function!!\n");
      exit(EXIT_FAILURE);
    }

  int i;
  MddNode *MN;
  for(i = 0; i < hashSize; i++){
    //printf("%d\n",(hashFunction(mtbdd)+i) % hashSize);
    //printf("%d\n",(MN = hash[(hashFunction(mtbdd)+i) % hashSize]) != NULL);
    //printf("%d\n",MN -> ddnode == mtbdd);
    if( (MN = hash[(hashFunction(mtbdd)+i) % hashSize]) != NULL && MN -> ddnode == mtbdd)
      return MN;
  }
  
  printf("hash over flow error in get function!!\n");
  exit(EXIT_FAILURE);
  //return MN;
}

/* int* intToArray(int x,int k){ */
/*   int i,array[k]; */
/*   for(i=k-1;0<=i;i--){ */
/*     array[i]= x%2; */
/*     x/=2; */
/*   } */

/*   return array; */
/* } */

MddNode* makeMDDNode(int k){
  
  MddNode *mddNode;
  
  if(( mddNode = (MddNode*)malloc(sizeof(MddNode))) == NULL){
    printf("memory for mddNode error in makeMDDNode function!!\n");
    exit(EXIT_FAILURE);
  }

  if(( mddNode->next = (MddNode**)malloc(sizeof(MddNode*)*(1<<k))) == NULL){
    printf("memory for next of mddNode error in makeMDDNode function!!\n");
    exit(EXIT_FAILURE);
  }

  return mddNode;
}

MddNode* makeMDD(int k,DdManager *gbm,DdNode *mtbdd){
  //  Cudd_IsNonConstant(mtbdd) 
  if(Cudd_IsNonConstant(mtbdd) && doesNotExist(mtbdd)){
    MddNode *mddNode = makeMDDNode(k);
    mddNode -> k = k;
    unsigned int idx = Cudd_NodeReadIndex(mtbdd), remainder = idx%k;
    mddNode -> index = remainder == 0 ? idx : idx-remainder ;
    mddNode -> value = -1;
    mddNode -> ddnode = mtbdd;
    int i,j,array[k],quotient,subremainder;
    DdNode *descendant;

    for(i = 0; i < 1<<k; i++){
      descendant = mtbdd;
      quotient = i;
      subremainder = remainder;
      for(j=k-1;0<=j;j--){
	array[j] = quotient%2;
	quotient/=2;
      }
      for(j=0;j<k;j++){
	if(subremainder!=0){
	  subremainder--;
	  continue;
	}
	array[j] ? descendant = Cudd_T(descendant) : descendant = Cudd_E(descendant);
	if(Cudd_IsConstant(descendant) || Cudd_NodeReadIndex(descendant) >= idx-remainder+k)
	  break;
      }
      mddNode -> next[i] = makeMDD(k,gbm,descendant); 
    }
    //printf("%u\n",hashFunction(mtbdd));
    set(mddNode);
  }
  return get(mtbdd);
}

MddNode* MDD_Eval(int bitNum,int k,MddNode *mdd,int *array){

  // printf("%d%d%d%d%d%d\n",array[0],array[1],array[2],array[3],array[4],array[5]);
  while(mdd->value == -1){
    mdd = mdd->next[array[(mdd->index)/k]];
  }
  return mdd;

}

/* int DFS(MddNode *mdd){ */
  
  
  
  
/* } */

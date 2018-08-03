#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cudd-3.0.0/cudd/cudd.h"
#include "include/DdList.h"
#include "include/MDD.h"
#include <sys/time.h>
#include <time.h>

DdNode* makeDdNode(DdManager *gbm, char *rule){
  
  DdNode *bdd,*var,*tmp;
  int i;

  bdd = Cudd_ReadOne(gbm); /*Returns the logic one constant of the manager*/
  Cudd_Ref(bdd);

  for(i = 0; i < strlen(rule)-1; i++){
    if(rule[i] == '*')
      continue;
    switch(rule[i])
      {
      case '1':
	var = Cudd_bddIthVar(gbm,i);
	break;
      case '0':
	var = Cudd_Not(Cudd_bddIthVar(gbm,i));
	break;
      }
    tmp = Cudd_bddAnd(gbm, bdd, var); /*Perform AND Boolean operation*/
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(gbm,bdd);
    bdd = tmp;
  }

  return bdd;
}

/* ./MyMain[0] RuleListFile[1] BitNumberOfAggregation[2] HeaderListFile[3] bitlegth[4] */
int main(int argc, char *argv[]) /* ./MyMain RuleListFile NumberOfRules BitNumberOfAggregation HeaderListFile NumberOfHeaders bitlegth*/
{
  struct timeval start,timeofmtbdd,timeofmdd,clatimeofmtbdd,clatimeofmdd;
  //--------------------------------------------------------------- make MTBDD from RuleList

  FILE *fp,*fp2;
  int i;

  char def[atoi(argv[4])+2];
  for(i = 0; i < atoi(argv[4]);i++)
    def[i] = '*';
  def[i++] = '\n';
  def[i] = '\0';

  if((fp = fopen(argv[1],"r")) == NULL){
    printf("RuleList file open error!!\n");
    exit(EXIT_FAILURE);
  }

  char rule[256],cheader[256]/*,filename[30]="mmresult.dot"*/;
  /* int size = atoi(argv[2]); */
  int size = 0;
  for( ; fgets(rule,256,fp) != NULL; ++size)
    ;

  if((fp2 = fopen(argv[3],"r")) == NULL){
    printf("HeaderList file open error!!\n");
    exit(EXIT_FAILURE);
  }

  /* int headerSize = atoi(argv[5]); */
  int headerSize = 0;
  for( ;fgets(cheader,256,fp2) != NULL; ++headerSize)
    ;

  // printf("size = %d, atoi(argv[2]) = %d, headerSize = %d, argv[5] = %d\n", size, atoi(argv[2]), headerSize, atoi(argv[5]));

  int *headerList[headerSize],*header, bitlen = atoi(argv[4]), k = atoi(argv[2]);
  DdManager *gbm;
  DdNode *bdd,*mtbdd,*submtbdd,*tmp,*tmp2;
  DdList *terminalList,*terminalIndex,*fwdIndex;// for MDD

  if((terminalList = (DdList*)malloc(sizeof(DdList))) == NULL){ //for MDD
    printf("memory for terminalList error!!\n");
    exit(EXIT_FAILURE);
  }
  terminalIndex = terminalList; // for MDD

  gbm = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0); /* Initialize a new BDD manager. */
  
  mtbdd = Cudd_ReadZero(gbm);/*Returns the arithmetic zero constant of the manager*/
  Cudd_Ref(mtbdd);

  rewind(fp);
  for(i = 1; i <= size+1; i++){
    if(i != size+1)
      fgets(rule,256,fp);
    else
      strcpy(rule,def);    
    terminalIndex->node = Cudd_addConst(gbm,i);
    Cudd_Ref(terminalIndex->node);
    bdd = makeDdNode(gbm,rule);
    tmp = Cudd_BddToAdd(gbm,bdd);
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(gbm,bdd);
    bdd = tmp;
    submtbdd = Cudd_addApply(gbm,Cudd_addTimes,terminalIndex->node,bdd);
    Cudd_Ref(submtbdd);
    Cudd_RecursiveDeref(gbm,bdd);
      
    tmp2 = Cudd_addApply(gbm,Cudd_addUni,mtbdd,submtbdd);
    Cudd_Ref(tmp2);
    Cudd_RecursiveDeref(gbm,mtbdd);
    Cudd_RecursiveDeref(gbm,submtbdd);
    mtbdd = tmp2;
    if((terminalIndex -> next = (DdList*)malloc(sizeof(DdList))) == NULL){ //for MDD
      printf("memory for terminalIndex error!!\n");
      exit(EXIT_FAILURE);
    }
    fwdIndex = terminalIndex; // for MDD
    terminalIndex = terminalIndex -> next; // for MDD
  }
  free(terminalIndex);
  fwdIndex -> next = NULL;
  fclose(fp);  

  rewind(fp2);
  int sum,h,j;
  for(i=0;fgets(cheader,256,fp2) != NULL;i++){
    int pkt[bitlen];
    if(( header = (int*)malloc(sizeof(int)*((int)(bitlen/k+0.9)))) == NULL){
      printf("memory for header error!!\n");
      exit(EXIT_FAILURE);
    }
    sum = 0;
    h = k-1;
    for(j=0;j<bitlen;j++){
      pkt[j] = cheader[j] - '0';
      sum += pkt[j]*(1<<h);
      if(h == 0){
	header[j/k] = sum;
	sum = 0;
	h = k-1;
      }
      else
	h--;
    }
    if(h != k-1)
      header[(j-1)/k] = sum;
    //printf("%d\n",(int)Cudd_V(Cudd_Eval(gbm,mtbdd,pkt)));
    if( Cudd_IsConstant(Cudd_Eval(gbm,mtbdd,pkt)) != 1){
      printf("mtbdd classfication error!! \n");
      exit(EXIT_FAILURE);
    }
    headerList[i] = header;
  }
  fclose(fp2);

  //------------------------------------------------------------------- make MDD from MTBDD
  gettimeofday(&start,NULL);
  
  unsigned int hashSize = Cudd_DagSize(mtbdd)*10;
  keepMemoryForHash(hashSize,terminalList,k);
  free_behaviors(terminalList);
  MddNode *mdd;
  mdd = makeMDD(k,gbm,mtbdd);

  gettimeofday(&timeofmdd,NULL);
  printf("time of making MDD = %lf s\n",(timeofmdd.tv_sec - start.tv_sec) + (timeofmdd.tv_usec - start.tv_usec)*1.0e-6 );

  struct timespec s, e;
  clock_gettime(CLOCK_REALTIME, &s);
  for(i=0;i<headerSize;i++){
    //printf("[%d,%d,%d]\n",*(headerList[i]),*(headerList[i]+1),*(headerList[i]+2));
    //printf("%d\n",MDD_Eval(bitlen,k,mdd,headerList[i])->value);
    if( MDD_Eval(bitlen,k,mdd,headerList[i])->value == -1){
      printf("mdd classfication error!! \n");
      exit(EXIT_FAILURE);
    }
  }
  clock_gettime(CLOCK_REALTIME, &e);
  gettimeofday(&clatimeofmdd,NULL);

  if (e.tv_nsec < s.tv_nsec) {
    printf("MDD Search Time = %10ld.%09ld\n", e.tv_sec - s.tv_sec - 1, e.tv_nsec + 1000000000 - s.tv_nsec);
  } else {
    printf("MDD Search Time = %10ld.%09ld\n", e.tv_sec - s.tv_sec, e.tv_nsec - s.tv_nsec);
  }    
  
  /* printf("classfication time of MDD = %lf ms\n",(clatimeofmdd.tv_sec - timeofmdd.tv_sec)*1000 + (clatimeofmdd.tv_usec - timeofmdd.tv_usec)*1.0e-3 ); */
  
  /* int j,bitNum = 6,buhi[bitNum],header; //enumerate all rule number for all packet by mdd */
  /* for(i=0;i< 1<<bitNum;i++){ */
  /*    header = i; */
  /*    for(j=bitNum-1;0<=j;j--){ */
  /*      buhi[j] = header%2; */
  /*      header/=2; */
  /*    } */
  /*    printf("%d%d%d%d%d%d ->  %d\n",buhi[0],buhi[1],buhi[2],buhi[3],buhi[4],buhi[5],(MDD_Eval(bitNum,k,mdd,buhi))->value); */
  /* } */

  //-------------------------------------------------------------------

  Cudd_Quit(gbm);

  return 0;
}

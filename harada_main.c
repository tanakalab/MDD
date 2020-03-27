#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cudd-3.0.0/cudd/cudd.h"
#include "include/DdList.h"
#include "include/MDD.h"
#include "include/rule.h"
#include "include/header.h"
#include "include/rbt.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

void write_dd(DdManager* gm, DdNode *dd, char* filename) {
    FILE *outfile; // output file pointer for .dot file
    outfile = fopen(filename,"w");
    DdNode **ddnodearray = (DdNode**)malloc(sizeof(DdNode*)); // initialize the function array
    ddnodearray[0] = dd;
    Cudd_DumpDot(gm, 1, ddnodearray, NULL, NULL, outfile); // dump the function to .dot file
    free(ddnodearray);
    fclose (outfile); // close the file */
}


DdNode* makeDdNode(DdManager *gbm, char *rule){
  
  DdNode *bdd, *var, *tmp;
  int i;

  bdd = Cudd_ReadOne(gbm); /*Returns the logic one constant of the manager*/
  Cudd_Ref(bdd);

  for(i = 0; i < (int)strlen(rule)-1; i++){
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
  //--------------------------------------------------------------- make MTBDD from RuleList


  /****** 引数の数とルールリストファイルのチェック開始 *****/
  if (4 != argc) {
    fprintf(stderr, "ERROR!! The number of arguments is invalid\n");
    exit(1);
  }
  FILE *fp;
  if((fp = fopen(argv[1],"r")) == NULL){
    printf("RuleList file open error!!\n");
    exit(EXIT_FAILURE);
  }
  /****** 引数の数とルールリストファイルのチェック終了 *****/

  /****** ルール数 (ruleSize) のカウント開始 *****/
  char dummy[255], cheader[255] /*, filename[30]="mmresult.dot" */;
  /* int ruleSize = atoi(argv[2]); */
  int ruleSize = 0;
  for( ; fgets(dummy, 256, fp) != NULL; ++ruleSize) { ; }
  /****** ルール数 (ruleSize) のカウント終了 *****/

  /****** ヘッダファイルのチェックとヘッダ数 (headerSize) のカウント開始 *****/
  FILE *fp2;
  if((fp2 = fopen(argv[3],"r")) == NULL){
    printf("HeaderList file open error!!\n");
    exit(EXIT_FAILURE);
  }
  int headerSize = 0;
  for( ;fgets(cheader,256,fp2) != NULL; ++headerSize) { ; }
  /****** ヘッダファイルのチェックとヘッダ数 (headerSize) のカウント終了 *****/
  /* printf("ruleSize = %d, atoi(argv[2]) = %d, headerSize = %d\n", ruleSize, atoi(argv[2]), headerSize); */

  /****** MTBDD作成準備 ******/
  DdManager *gbm;
  DdNode *bdd, *mtbdd, *submtbdd, *tmp, *tmp2;
  DdList *terminalList, *terminalIndex, *fwdIndex; // for MDD

  if(NULL == (terminalList = (DdList*)malloc(sizeof(DdList)))) { //for MDD
    fprintf(stderr, "memory for terminalList error!!\n");
    exit(EXIT_FAILURE);
  }
  terminalIndex = terminalList; // for MDD

  gbm = Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0); /* Initialize a new BDD manager. */
  mtbdd = Cudd_ReadZero(gbm);/*Returns the arithmetic zero constant of the manager*/
  Cudd_Ref(mtbdd);

  rewind(fp);
  int i;
  struct timeval start, timeofmdd, clatimeofmdd;
  gettimeofday(&start,NULL);

  for(i = 1; i <= ruleSize; i++) {
    int SA1, SA2, SA3, SA4, SAPrefix, DA1, DA2, DA3, DA4, DAPrefix;
    int SPL, SPR, DPL, DPR, PROT, PROTMask;
    char FLAG[255];
    fscanf(fp,"@%d.%d.%d.%d/%d %d.%d.%d.%d/%d %d : %d %d : %d %x/%x %s\n", &SA1, &SA2, &SA3, &SA4, &SAPrefix, &DA1, &DA2, &DA3, &DA4, &DAPrefix, &SPL, &SPR, &DPL, &DPR, &PROT, &PROTMask, FLAG);
    rule* r = mk_rule(i, SA1, SA2, SA3, SA4, SAPrefix, DA1, DA2, DA3, DA4, DAPrefix, SPL, SPR, DPL, DPR, PROT, PROTMask);
    /* rule_print(r); putchar('\n'); */
    char zms[105]; zms[104] = '\0';
    unsigned j;
    /**** SA and DA 送信元アドレス(32bit)と宛先アドレス(32bit)分の0,1,*の文字列を zms にコピー ****/
    for (j = 0; j < 32; ++j)
      zms[j] = r->sa[j], zms[j+32] = r->da[j];
    /**** PROT プロトコル(8bit)分の0,1,*の文字列を zms にコピー ****/
    for (j = 0; j < 8; ++j)
      zms[PROT_START_POINT+j] = r->prot[j];
    /**** SP and DP 送信元ポート番号(16bit)と宛先ポート番号(16bit)分の 0,1,*の文字列をコピー *****/
    /*** ポート番号は x : y の形であり，一つの 0,1,* の文字列では表現できないことがあることに注意 ***/
    /** 送信元ポート番号を 0,1,* の文字列のリストへ (ls) と変換 **/
    list_string* ls = range_to_01ms(r->spl, r->spr, DEFAULTPORTNUMBER_L, DEFAULTPORTNUMBER_R);
    /** 送信元ポート番号を 0,1,* の文字列のリストへ (ld) と変換 **/
    list_string* ld = range_to_01ms(r->dpl, r->dpr, DEFAULTPORTNUMBER_L, DEFAULTPORTNUMBER_R);
    list_string_cell* ptr_s;
    /* 送信元ポート番号の文字列と宛先ポート番号の文字列それぞれでループを回す */
    for (ptr_s = ls->head; NULL != ptr_s; ptr_s = ptr_s->next) {
      for (j = 0; j < 16; ++j)
	zms[SP_START_POINT+j] = ptr_s->key[j]; 
      list_string_cell* ptr_d;
      for (ptr_d = ld->head; NULL != ptr_d; ptr_d = ptr_d->next) {
	for (j = 0; j < 16; ++j)
	  zms[DP_START_POINT+j] = ptr_d->key[j];
	/* printf("zms = %s\n", zms); */
	terminalIndex->node = Cudd_addConst(gbm, i); /* 終端節点 i を追加 */
	Cudd_Ref(terminalIndex->node);
	bdd = makeDdNode(gbm, zms); /* ルール zms に対する BDD を構築 */
	tmp = Cudd_BddToAdd(gbm, bdd);
	Cudd_Ref(tmp);
	Cudd_RecursiveDeref(gbm, bdd);
	bdd = tmp;
	submtbdd = Cudd_addApply(gbm, Cudd_addTimes, terminalIndex->node, bdd); /* MTBDD へと変換 */
	Cudd_Ref(submtbdd);
	Cudd_RecursiveDeref(gbm, bdd);
      
	tmp2 = Cudd_addApply(gbm, Cudd_addUni, mtbdd, submtbdd); /* 既存の MTBDD とマージ */
	Cudd_Ref(tmp2);
	Cudd_RecursiveDeref(gbm,mtbdd);
	Cudd_RecursiveDeref(gbm,submtbdd);
	mtbdd = tmp2;
	if((terminalIndex->next = (DdList*)malloc(sizeof(DdList))) == NULL){ //for MDD
	  fprintf(stderr, "memory for terminalIndex error!!\n");
	  exit(EXIT_FAILURE);
	}
	fwdIndex = terminalIndex; // for MDD
	terminalIndex = terminalIndex -> next; // for MDD
      }
    }
    list_string_clear(ld); ld = NULL;
    list_string_clear(ls); ls = NULL;
    rule_clear(r); r = NULL;
  }

  /***** デフォルトルールの作成開始
	 （入力ルールリストファイルにデフォルトルールは含まれないことに注意 *****/
  const int bitlen = 104;
  char def[bitlen+1];
  for(i = 0; i < bitlen; i++)
    def[i] = '*';
  def[i] = '\0';

  terminalIndex->node = Cudd_addConst(gbm, i);
  Cudd_Ref(terminalIndex->node);
  bdd = makeDdNode(gbm, def);
  tmp = Cudd_BddToAdd(gbm, bdd);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(gbm, bdd);
  bdd = tmp;
  submtbdd = Cudd_addApply(gbm, Cudd_addTimes, terminalIndex->node, bdd);
  Cudd_Ref(submtbdd);
  Cudd_RecursiveDeref(gbm, bdd);
      
  tmp2 = Cudd_addApply(gbm, Cudd_addUni, mtbdd, submtbdd);
  Cudd_Ref(tmp2);
  Cudd_RecursiveDeref(gbm,mtbdd);
  Cudd_RecursiveDeref(gbm,submtbdd);
  mtbdd = tmp2;
  if((terminalIndex -> next = (DdList*)malloc(sizeof(DdList))) == NULL){ //for MDD
    fprintf(stderr, "memory for terminalIndex error!!\n");
    exit(EXIT_FAILURE);
  }
  fwdIndex = terminalIndex; // for MDD
  terminalIndex = terminalIndex -> next; // for MDD

  /***** デフォルトルール作成終了 *****/
  
  free(terminalIndex);
  fwdIndex->next = NULL;
  fclose(fp);

  /* write_dd(gbm, mtbdd, "mtbdd.dot"); */

  /* ヘッダリストの読み込み */
  int k = atoi(argv[2]);
  int *headerList[headerSize], *iheader;
  rewind(fp2);
  int sum, h, j;
  long SA, DA, SP, DP, PROT, FLAG;
  for (i = 0; EOF != fscanf(fp2, "%ld %ld %ld %ld %ld %ld", &SA, &DA, &SP, &DP, &PROT, &FLAG); ++i) {
    header* hh = mk_header(i, SA, DA, SP, DP, PROT);
    int pkt[bitlen];
    if(NULL == (iheader = (int*)malloc(sizeof(int)*((int)(bitlen/k+0.9))))) {
      printf("memory for header error!!\n");
      exit(EXIT_FAILURE);
    }
    sum = 0;
    h = k-1;
    for(j = 0; j < bitlen; j++) {
      // pkt[j] = cheader[j] - '0';
      pkt[j] = hh->string[j] - '0';
      sum += pkt[j]*(1<<h);
      if(h == 0){
  	iheader[j/k] = sum;
  	sum = 0;
  	h = k-1;
      }
      else
  	h--;
    }
    if(h != k-1)
      iheader[(j-1)/k] = sum;
    //printf("%d\n",(int)Cudd_V(Cudd_Eval(gbm,mtbdd,pkt)));
    if( Cudd_IsConstant(Cudd_Eval(gbm,mtbdd,pkt)) != 1){
      printf("mtbdd classfication error!! \n");
      exit(EXIT_FAILURE);
    }
    headerList[i] = iheader;
    header_clear(hh); hh = NULL;
  }
  fclose(fp2);

  /* ------------------------------------------------------------------- make MDD from MTBDD */  
  unsigned int hashSize = Cudd_DagSize(mtbdd)*10;
  keepMemoryForHash(hashSize, terminalList, k);
  free_behaviors(terminalList);
  MddNode *mdd;
  mdd = makeMDD(k, gbm, mtbdd);

  gettimeofday(&timeofmdd,NULL);
  printf("time of making MDD = %lf s\n",(timeofmdd.tv_sec - start.tv_sec) + (timeofmdd.tv_usec - start.tv_usec)*1.0e-6 );

  struct timespec s, e;
  clock_gettime(CLOCK_REALTIME, &s);
  for (i = 0; i < headerSize; i++) {
    //printf("[%d,%d,%d]\n",*(headerList[i]),*(headerList[i]+1),*(headerList[i]+2));
    //printf("%d\n",MDD_Eval(bitlen,k,mdd,headerList[i])->value);
    if (-1 == MDD_Eval(bitlen,k,mdd,headerList[i])->value) {
      printf("mdd classfication error!! \n");
      exit(EXIT_FAILURE);
    }
  }
  clock_gettime(CLOCK_REALTIME, &e);
  gettimeofday(&clatimeofmdd, NULL);

  if (e.tv_nsec < s.tv_nsec) {
    printf("MDD Search Time = %10ld.%09ld\n", e.tv_sec - s.tv_sec - 1, e.tv_nsec + 1000000000 - s.tv_nsec);
  } else {
    printf("MDD Search Time = %10ld.%09ld\n", e.tv_sec - s.tv_sec, e.tv_nsec - s.tv_nsec);
  }

  struct rusage mdd_mem;
  getrusage(RUSAGE_SELF, &mdd_mem);
  printf("Memory Usage: %ld (kB)\n", mdd_mem.ru_maxrss); //キロバイト単位
  
  /* /\* printf("classfication time of MDD = %lf ms\n",(clatimeofmdd.tv_sec - timeofmdd.tv_sec)*1000 + (clatimeofmdd.tv_usec - timeofmdd.tv_usec)*1.0e-3 ); *\/ */
  
  /* /\* int j,bitNum = 6,buhi[bitNum],header; //enumerate all rule number for all packet by mdd *\/ */
  /* /\* for(i=0;i< 1<<bitNum;i++){ *\/ */
  /* /\*    header = i; *\/ */
  /* /\*    for(j=bitNum-1;0<=j;j--){ *\/ */
  /* /\*      buhi[j] = header%2; *\/ */
  /* /\*      header/=2; *\/ */
  /* /\*    } *\/ */
  /* /\*    printf("%d%d%d%d%d%d ->  %d\n",buhi[0],buhi[1],buhi[2],buhi[3],buhi[4],buhi[5],(MDD_Eval(bitNum,k,mdd,buhi))->value); *\/ */
  /* /\* } *\/ */

  /* //------------------------------------------------------------------- */

  Cudd_Quit(gbm);

  return 0;
}

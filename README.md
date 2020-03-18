# MDD

## INSTRUCTION
$ git clone https://github.com/tanakalab/MDD.git  
$ cd MDD  
$ tar xvf cudd-3.0.0.tar.gz  
$ cd cudd-3.0.0  
$ autoreconf  
$ ./configure  
$ make  
$ cp cudd/.libs/libcudd.a ../  
$ cd ..  
$ make  

## Classification Example
./mdd Rule 2 Header 104          
(./mdd RuleListFile BitNumberOfAggregation HeaderListFile bitlength)


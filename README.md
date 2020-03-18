# MDD

## INSTRUCTION
$ git clone https://github.com/tanakalab/MDD.git	
$ cd MDD    
$ tar xvf
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


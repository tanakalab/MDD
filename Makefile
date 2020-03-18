CC	= g++ 
CFLAGS	= -g3 -Wall -Wextra
DEST	= ./
LDFLAGS = -L/usr/local/lib
LIBS	= -lm -lrt
OBJS	= main.o MDD.o DdList.o libcudd.a
PROGRAM = mdd

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:;	rm -f *.o *~ $(PROGRAM)

T1 = 2pl
T2 = s2pl

CC = g++
CFLAGS = -c -Wall -g -O2 -std=c++0x -Wunused-variable
LDFLAGS = -lpthread

O1 = 2pl.o lib.o
O2 = s2pl.o lib.o

#
# Rules for make
#
all: $(T2) $(T1) 

$(T1): $(O1) 
	$(CC) -o $@ $^ $(LDFLAGS)
$(T2): $(O2) 
	$(CC) -o $@ $^ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *~ *.o *.exe *.stackdump $(T1) $(T2)

T2 = occ

CC = g++
CFLAGS = -c -Wall -g -O0 -std=c++0x -Wunused-variable
LDFLAGS = -lpthread

O2 = occ.o lib.o

#
# Rules for make
#
all: $(T2) 

$(T2): $(O2) 
	$(CC) -o $@ $^ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *~ *.o *.exe *.stackdump $(T1) $(T2)

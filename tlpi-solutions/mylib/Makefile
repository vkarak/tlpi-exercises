.PHONY: all clean

all: mylib.a

include ../Makefile.inc

mylib.o: mylib.h mylib.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ mylib.c $(TLPI_LIB)

mylib.a: mylib.o
	ar crs mylib.a mylib.o

clean:
	$(RM_F) *.o mylib.a

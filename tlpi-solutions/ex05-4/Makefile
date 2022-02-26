.PHONY: all clean

include ../Makefile.inc


all: dup

dup: dup.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o dup

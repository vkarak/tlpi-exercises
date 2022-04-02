.PHONY: all clean

include ../Makefile.inc

all: buggy_longjmp

buggy_longjmp: buggy_longjmp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o buggy_longjmp

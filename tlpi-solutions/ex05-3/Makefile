.PHONY: all clean

include ../Makefile.inc


all: atomic_append

atomic_append: atomic_append.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o atomic_append

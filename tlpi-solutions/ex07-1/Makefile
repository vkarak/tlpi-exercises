.PHONY: all clean

include ../Makefile.inc


all: free_and_sbrk

free_and_sbrk: free_and_sbrk.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o free_and_sbrk

.PHONY: all clean

include ../Makefile.inc


all: tee

tee: tee.c
	$(CC) $(CFLAGS) -o $@ $< $(TLPI_LIB)

clean:
	$(RM_F) *.o tee

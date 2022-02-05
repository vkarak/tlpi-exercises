.PHONY: all clean

include ../Makefile.inc


all: tee

tee: tee.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o tee

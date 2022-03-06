.PHONY: all clean

include ../Makefile.inc


all: rwv

rwv: rwv.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o rwv

.PHONY: all clean

include ../Makefile.inc


all: cp

cp: cp.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o cp

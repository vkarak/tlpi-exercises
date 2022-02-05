.PHONY: all clean

include ../Makefile.inc


all: append_n_seek

append_n_seek: append_n_seek.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o cp

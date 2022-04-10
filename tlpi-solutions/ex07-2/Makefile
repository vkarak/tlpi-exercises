.PHONY: all clean

include ../Makefile.inc


all: xmalloc

xmalloc: xmalloc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB) -lcmocka

clean:
	$(RM_F) *.o xmalloc

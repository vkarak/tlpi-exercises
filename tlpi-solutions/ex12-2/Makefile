.PHONY: all clean

include ../Makefile.inc


all: proctree

proctree: proctree.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB) -lcmocka

clean:
	$(RM_F) *.o proctree

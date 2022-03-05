.PHONY: all clean

include ../Makefile.inc


all: dup_offset

dup_offset: dup_offset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o dup_offset

.PHONY: all clean

include ../Makefile.inc


all: mem_segments

mem_segments: mem_segments.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o mem_segments

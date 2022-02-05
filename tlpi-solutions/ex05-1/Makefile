.PHONY: all clean

include ../Makefile.inc


all: large_file

large_file: large_file.c
	$(CC) $(CPPFLAGS) -D_FILE_OFFSET_BITS=64 $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o large_file

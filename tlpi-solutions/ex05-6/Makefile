.PHONY: all clean

include ../Makefile.inc


all: dup_n_write

dup_n_write: dup_n_write.c
	$(CC) $(CPPFLAGS) -DKEEP_FILE $(CFLAGS) -o $@ $< $(MY_LIB) $(TLPI_LIB)

clean:
	$(RM_F) *.o dup_n_write

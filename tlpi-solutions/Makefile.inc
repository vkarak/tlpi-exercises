CC=gcc
CPPFLAGS=-D_GNU_SOURCE -I../mylib -I../../tlpi-dist/lib
CFLAGS=-g -std=c99 -Wextra -pedantic
TLPI_LIB=../../tlpi-dist/libtlpi.a
MY_LIB=../mylib/mylib.a
RM_F=/bin/rm -f

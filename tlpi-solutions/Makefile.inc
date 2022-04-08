CMOCKA_LIBDIR=$(shell pwd)/../../cmocka/build/src

CC=gcc
CPPFLAGS=-D_GNU_SOURCE -I../mylib -I../../tlpi-dist/lib -I../../cmocka/include
CFLAGS=-g -std=c99 -Wextra -pedantic
LDFLAGS=-L${CMOCKA_LIBDIR} -Wl,--rpath=$(CMOCKA_LIBDIR)
TLPI_LIB=../../tlpi-dist/libtlpi.a
MY_LIB=../mylib/mylib.a
RM_F=/bin/rm -f

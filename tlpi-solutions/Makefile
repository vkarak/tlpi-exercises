.PHONY: all clean

DIRS = \
	mylib \
	ex04-1 ex04-2 \
	ex05-1 ex05-2 ex05-3 ex05-4 ex05-5 ex05-6 ex05-7 \
	ex06-1 ex06-2 ex06-3 \
	ex07-1 ex07-2 \
	ex08-1 ex08-2 \
	ex09-3 \
	ex12-1 ex12-2

all:
	@for d in ${DIRS}; do ${MAKE} -C $$d all || break; done


clean:
	@for d in ${DIRS}; do ${MAKE} -C $$d clean || break; done

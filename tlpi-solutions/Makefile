.PHONY: all clean

DIRS = \
	mylib \
	ex04-1 ex04-2 \
	ex05-1 ex05-2 ex05-3

all:
	@for d in ${DIRS}; do ${MAKE} -C $$d all || break; done


clean:
	@for d in ${DIRS}; do ${MAKE} -C $$d clean || break; done

.PHONY: all clean

DIRS = mylib ex04-1

all:
	@for d in ${DIRS}; do ${MAKE} -C $$d all || break; done


clean:
	@for d in ${DIRS}; do ${MAKE} -C $$d clean || break; done

all: fatfs1

fatfs1: ../src/ff.c ../src/option/ccsbcs.c util.c fatfs1.c
	gcc -g -I.. -std=c99 -DFFCONF_H='"tests/fatfs1_conf.h"' $^ -o $@

test: fatfs1
	./fatfs1 | diff - fatfs1.exp

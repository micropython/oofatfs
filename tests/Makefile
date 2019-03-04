all: fatfs1 fatfs2

fatfs1: ../src/ff.c ../src/ffunicode.c util.c fatfs1.c
	gcc -g -I.. -std=c99 -DFFCONF_H='"tests/fatfs1_conf.h"' $^ -o $@

fatfs2: ../src/ff.c ../src/ffunicode.c util.c fatfs2.c
	gcc -g -I.. -std=c99 -DFFCONF_H='"tests/fatfs2_conf.h"' $^ -o $@

test: fatfs1 fatfs2
	./fatfs1 | diff - fatfs1.exp
	./fatfs2 | diff - fatfs2.exp

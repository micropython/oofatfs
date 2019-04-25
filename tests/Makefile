all: fatfs1 fatfs2

fatfs1: ../src/ff.c ../src/ffunicode.c util.c fatfs1.c
	gcc -g -I.. -std=c99 -DFFCONF_H='"tests/fatfs1_conf.h"' $^ -o $@

fatfs2: ../src/ff.c ../src/ffunicode.c util.c fatfs2.c
	gcc -g -I.. -std=c99 -DFFCONF_H='"tests/fatfs2_conf.h"' $^ -o $@

fatfs_repair: ../src/ff.c ../src/ffunicode.c util.c fatfs_repair.c
	gcc -g -I.. -std=c99 -Werror -DFFCONF_H='"tests/fatfs_repair_conf.h"' $^ -o $@
	./fatfs_repair

fatfs_repair_img: ../src/ff.c ../src/ffunicode.c util.c fatfs_repair_img.c
	gcc -g -I.. -std=c99 -Werror -DFFCONF_H='"tests/fatfs_repair_conf.h"' $^ -o $@

	zcat test.full.img.gz > /tmp/test.img
	fsck.vfat -n -l /tmp/test.img || true
	./fatfs_repair_img /tmp/test.img
	fsck.vfat -n -l /tmp/test.img || true
	rm /tmp/test.img

test: fatfs1 fatfs2
	./fatfs1 | diff - fatfs1.exp
	./fatfs2 | diff - fatfs2.exp

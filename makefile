all: diffdir379

diffdir379: cmput379.c readdir.c
	gcc -std=c99 cmput379.c readdir.c -o diffdir379

clean:
	rm -f diffdir379

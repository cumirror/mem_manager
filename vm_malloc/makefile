test: test.o memlib.o mm.o
	gcc test.o memlib.o mm.o -o test

test.o: test.c
	gcc -c test.c

mm.o: mm.c
	gcc -c mm.c

memlib.o: memlib.c
	gcc -c memlib.c

clean:
	-rm test *.o

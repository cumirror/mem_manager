test: test.o memlib.o mm.o
	gcc test.o memlib.o mm.o -o test

test.o: test.c
	gcc -c test.c

mm.o: vm_malloc/mm.c
	gcc -c vm_malloc/mm.c

memlib.o: vm_malloc/memlib.c
	gcc -c vm_malloc/memlib.c

clean:
	-rm test *.o

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static char* mem_heap;
static char* mem_brk;
static char* mem_max_addr;

#define MAX_HEAP (1<<20)

int mem_init()
{
	mem_heap = (char*)malloc(MAX_HEAP);

	if (mem_heap == NULL) {
		return -1;
	}

	mem_brk = mem_heap;
	mem_max_addr = mem_brk + MAX_HEAP;
	return 0; 
}

void* mem_sbrk(int incr)
{
	char* old_brk = mem_brk;

	if (incr < 0 || (mem_brk + incr) > mem_max_addr) {
		errno = ENOMEM;
		fprintf(stderr, "ERROR: mem_sbrk failed. Out of Memory...\n");
		return (void*)-1;
	}

	mem_brk += incr;
	return (void*)old_brk;
}

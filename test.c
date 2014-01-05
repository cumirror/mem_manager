#include <stdio.h>

extern int mm_init();
extern void* mm_malloc(size_t size);
extern void mm_free(void* ptr);

int main(int argc, char** argv)
{
	int count = 0;
	void* x;

	if (mm_init() < 0) {
		printf("Init memory error...\n");
		return -1;
	}

//	(4K/112)*(4M/4K - 1) = 9180
//	while(mm_malloc(100)) count++;

//	how to compute here ...
	while (x = mm_malloc(100)) {
		count++;
		if (count%5 == 0) {
			mm_free(x);
		}
	}

	printf("malloc count: %d\n", count);
	return 0;
}

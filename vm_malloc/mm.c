#include <stddef.h>

extern int mem_init();
extern void* mem_sbrk(int incr);

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x,y) ((x)>(y)?(x):(y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))

static void* heap_listp;

static void* coalesce(void* bp)
{
	char prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
	char next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp)); 

	if (prev_alloc && next_alloc) {
		;
	} else if (prev_alloc && !next_alloc) {
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	} else if (!prev_alloc && next_alloc) {
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	} else {
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	return bp;
}

static void* extend_heap(size_t words)
{
	char* bp;
	size_t size;

	size = words % 2 ? (words + 1) * WSIZE : words * WSIZE;

	if ((long)(bp = mem_sbrk(size)) == -1) {
		return NULL;
	}

	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

	return coalesce(bp);
}

static void* find_fit(size_t asize)
{
	char* bp = heap_listp;
	while (GET_SIZE(HDRP(bp)) != 0) {
		if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize)
			return bp;
		bp = NEXT_BLKP(bp);
	}

	return NULL;
}

static void place(void* bp, size_t asize)
{
	size_t size;

	size = GET_SIZE(HDRP(bp));
	if ((size - asize) <= DSIZE) {
		PUT(HDRP(bp), PACK(size, 1));
		PUT(FTRP(bp), PACK(size, 1));
	} else {
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		PUT(HDRP(NEXT_BLKP(bp)), PACK(size - asize, 0));
		PUT(HDRP(NEXT_BLKP(bp)), PACK(size - asize, 0));
	}
}

int mm_init()
{
	if (mem_init() < 0)
		return -1;
	if ((heap_listp = mem_sbrk(4*WSIZE)) == (void*)-1)
		return -1;

	PUT(heap_listp, 0);
	PUT(heap_listp + 1*WSIZE, PACK(DSIZE, 1));
	PUT(heap_listp + 2*WSIZE, PACK(DSIZE, 1));
	PUT(heap_listp + 3*WSIZE, PACK(0, 1));

	heap_listp += 2*WSIZE;

	if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
		return -1;

	return 0;
}

void mm_free(void* bp)
{
	size_t size = GET_SIZE(HDRP(bp));

	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));

	coalesce(bp);
}

void* mm_malloc(size_t size)
{
	size_t asize;
	size_t extendsize;
	char* bp;

	if (size == 0)
		return NULL;

	if (size <= DSIZE)
		asize = 2*DSIZE;
	else
		asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);

	if ((bp = find_fit(asize)) != NULL) {
		place(bp, asize);
		return bp;
	}
	
	extendsize = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;

	place(bp, asize);
	return bp;
}

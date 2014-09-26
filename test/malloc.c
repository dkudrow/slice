#include <stdio.h>

#include "malloc.h"

#define HEAP_SIZE 1024

char HEAP[HEAP_SIZE];

int main(void) {
	int i;
	void *ptrs[8];

	printf("Initalizing heap\n================\n");
	malloc_init(HEAP, HEAP_SIZE);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Freeing random memory\n=====================\n");
	free(HEAP);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Making 8 allocations of 64 bytes\n================================\n");
	for (i=0; i<8; i++)
		ptrs[i] = malloc(64);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Freeing 4 allocations of 64 bytes\n=================================\n");
	for (i=1; i<8; i+=2)
		free(ptrs[i]);
	malloc_dump(HEAP, HEAP_SIZE);

	printf("Freeing 4 allocations of 64 bytes\n=================================\n");
	for (i=0; i<8; i+=2)
		free(ptrs[i]);
	malloc_dump(HEAP, HEAP_SIZE);

	return 0;
}

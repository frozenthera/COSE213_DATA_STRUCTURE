#include <stdio.h>
#include <stdlib.h> // malloc, realloc

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp( HEAP *heap, int index){
	if(index == 0) return;
	if(heap->compare(*(heap->heapArr+((index-1)/2)), *(heap->heapArr+index)) < 0){
		void *temp = *(heap->heapArr+index);
		*(heap->heapArr+index) = *(heap->heapArr+((index-1)/2));
		*(heap->heapArr+((index-1)/2)) = temp;
		_reheapUp(heap, (index-1)/2);
	}
}


/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index){
	int nextIdx;
	void *largest;
	if(index*2+1 <= heap->last){
		void *left = *(heap->heapArr+(index*2+1));
		nextIdx = index*2+1;
		largest = left;
		if(index*2+2 <= heap->last){		
			void *right = *(heap->heapArr+(index*2+2));
			if(heap->compare(left, right) < 0){
				nextIdx = index*2+2;
				largest = right;
			}
		}
		if(heap->compare(largest, *(heap->heapArr+index)) > 0){
			*(heap->heapArr+nextIdx) = *(heap->heapArr+index);
			*(heap->heapArr+index) = largest;
			_reheapDown(heap, nextIdx);
		}
	}
}	

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP *heap_Create( int capacity, int (*compare) (void *arg1, void *arg2)){
	HEAP *ptr = (HEAP *)malloc(sizeof(HEAP));
	ptr->compare = compare;
	ptr->capacity = capacity;
	ptr->last = -1;
	ptr->heapArr = malloc(sizeof(void *) * capacity);
}

/* Free memory for heap
*/
void heap_Destroy( HEAP *heap){
	for(int i=0; i<=heap->last; i++){
		free(*(heap->heapArr+i));
	}
	free(heap->heapArr);
	heap->capacity = 0;
	heap->last = 0;
	free(heap);
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert( HEAP *heap, void *dataPtr){
	if(heap->last+1 == heap->capacity) {
		if(heap->heapArr = (void *)realloc(heap->heapArr, sizeof(void *) * heap->capacity * 2)){
			heap->capacity *= 2;
		}
		else return 0;
	}
	heap->last++;
	*(heap->heapArr+heap->last) = dataPtr;	
	_reheapUp(heap, heap->last);
	return 1;
}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete( HEAP *heap, void **dataOutPtr){
	if(heap_Empty(heap) == 1) return 0;
	*dataOutPtr = *heap->heapArr;
	*(heap->heapArr) = *(heap->heapArr+heap->last);
	heap->last--;
	_reheapDown(heap, 0);
	return 1;
}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(  HEAP *heap){
	if(heap->last == -1) return 1;
	return 0;
}

/* Print heap array */
void heap_Print( HEAP *heap, void (*print_func) (void *data)){
	for(int i=0; i<=heap->last; i++){
		print_func(*(heap->heapArr+i));
	}
	printf("\n");
}
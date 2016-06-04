#include "pq.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct heap {
	int id;
	int heap_index;
	double priority;
} HEAP;

struct pq_struct {
	HEAP* heap;
	HEAP** id_ptrs;
	int size;
	int capacity;
	int is_min_heap;
};

PQ* pq_create(int capacity, int min_heap) {
	PQ* pq = malloc(sizeof(PQ));
	pq->heap = malloc(sizeof(HEAP)*(capacity+1));
	pq->id_ptrs = malloc(sizeof(HEAP*)*capacity);
	pq->size = 0;
	pq->capacity = capacity;
	pq->is_min_heap = min_heap;

	int i;
	for(i = 0; i < capacity; i++)
		pq->id_ptrs[i] = NULL;

	return pq;
}

void pq_free(PQ* pq) {
	if(pq->heap != NULL)
		free(pq->heap);
	if(pq->id_ptrs != NULL)
		free(pq->id_ptrs);
	if(pq != NULL)
		free(pq);
}

void pq_swap(PQ* pq, int index_1, int index_2) {
	int id_1 = pq->heap[index_1].id;
	double priority_1 = pq->heap[index_1].priority;

	int id_2 = pq->heap[index_2].id;
	double priority_2 = pq->heap[index_2].priority;

	pq->heap[index_1].id = id_2;
	pq->heap[index_1].priority = priority_2;

	pq->heap[index_2].id = id_1;
	pq->heap[index_2].priority = priority_1;

	pq->id_ptrs[id_1] = &(pq->heap[index_2]);
	pq->id_ptrs[id_2] = &(pq->heap[index_1]);
}

void percolate_down_max(PQ* pq, int index) {
	int max, done = 0;
	int left_index = 2*index;
	int right_index = left_index+1;

	while(left_index <= pq->size && !done) {
		max = left_index;
		if(right_index <= pq->size)
			if(pq->heap[left_index].priority < pq->heap[right_index].priority)
				max = right_index;
		if(pq->heap[max].priority > pq->heap[index].priority) {
			pq_swap(pq, max, index);
			index = max;
		}
		else
			done = 1;

		left_index = 2*index;
		right_index = left_index+1;
	}	
}

void percolate_down_min(PQ* pq, int index) {
	int min, done = 0;
	int left_index = 2*index;
	int right_index = left_index+1;

	while(left_index <= pq->size && !done) {
		min = left_index;

		if(right_index <= pq->size)
			if(pq->heap[left_index].priority > pq->heap[right_index].priority)
				min = right_index;

		if(pq->heap[min].priority < pq->heap[index].priority) {
			pq_swap(pq, min, index);
			index = min;
		}
		else
			done = 1;

		left_index = 2*index;
		right_index = left_index+1;
	}	
}

void percolate_down(PQ* pq, int index) {
	int parent_index = index/2;
	
	if(pq->is_min_heap)
		percolate_down_min(pq, index);
	else
		percolate_down_max(pq, index);
}

void percolate_up(PQ* pq, int index) {
	int parent_index = index/2;
	
	if(pq->is_min_heap) {
		while(parent_index >= 1 && pq->heap[parent_index].priority > pq->heap[index].priority) {
			pq_swap(pq, index, parent_index);
			index = parent_index;
			parent_index = index/2;
		}
	}
	else {
		while(parent_index >= 1 && pq->heap[parent_index].priority < pq->heap[index].priority) {
			pq_swap(pq, index, parent_index);
			index = parent_index;
			parent_index = index/2;
		}
	}
}

int pq_insert(PQ* pq, int id, double priority) {
	if(id < 0 || id >= pq->capacity) {
		printf("ID out of range.\n");
		return 0;
	}

	if(pq->id_ptrs[id] != NULL) {
		printf("ID in use.\n");
		return 0;
	}

	if(pq->size >= pq->capacity) {
		printf("Full is priority queue.\n");
		return 0;
	}

	int index = ++pq->size;
	pq->heap[index].id = id;
	pq->heap[index].heap_index = index;
	pq->heap[index].priority = priority;	// insert data at the end of the array
	pq->id_ptrs[id] = &(pq->heap[index]);		// label the id of the data as IDs

	percolate_up(pq, index);

	return 1;
}

int pq_change_priority(PQ* pq, int id, double new_priority) {
	if(id < 0 || id >= pq->capacity) {
		printf("ID out of range.\n");
		return 0;
	}

	if(pq->id_ptrs[id] == NULL) {
		printf("ID not in use.\n");
		return 0;
	}

	pq->id_ptrs[id]->priority = new_priority;

	int index = pq->id_ptrs[id]->heap_index;
	percolate_up(pq, index);
	percolate_down(pq, index);

	return 1;
}

int pq_remove_by_id(PQ* pq, int id) {
	if(id < 0 || id >= pq->capacity) {
		printf("ID out of range.\n");
		return 0;
	}

	if(pq->id_ptrs[id] == NULL) {
		printf("ID not in use.\n");
		return 0;
	}

	int pq_size = pq->size;
	if(pq_size == 0) {
		printf("Empty priority queue.\n");
		return 0;
	}

	if(pq_size == 1 || pq->id_ptrs[id]->heap_index == pq_size) {
		pq->id_ptrs[id] = NULL;		// remove the location label
		pq->size--;

		return 1;
	}

	int new_id;
	double new_priority;

	new_id = pq->heap[pq_size].id;								// copy data from the last data set
	new_priority = pq->heap[pq_size].priority;

	pq->id_ptrs[id]->id = new_id;								// replace the data from the id specified
	pq->id_ptrs[id]->priority = new_priority;
	pq->id_ptrs[new_id] = pq->id_ptrs[id];						// update the label location of the copied data
	
	pq->id_ptrs[id] = NULL;		// remove the location label
	pq->size--;

	int index = pq->id_ptrs[new_id]->heap_index;
	percolate_up(pq, index);
	percolate_down(pq, index);

	return 1;
}

int pq_get_priority(PQ* pq, int id, double *priority) {
	if(id < 0 || id >= pq->capacity) {
		printf("ID out of range.\n");
		return 0;
	}

	if(pq->id_ptrs[id] == NULL) {
		printf("ID not in use.\n");
		return 0;
	}

	*priority = pq->id_ptrs[id]->priority;

	return 1;
}

int pq_delete_top(PQ* pq, int *id, double *priority) {
	int pq_size = pq->size;
	if(pq_size == 0) {
		printf("Empty priority queue.\n");
		return 0;
	}

	*id = pq->heap[1].id;
	*priority = pq->heap[1].priority;

	if(pq_size > 1) {
		int new_id = pq->heap[pq_size].id;
		double new_priority = pq->heap[pq_size].priority;
		pq->heap[1].id = new_id;
		pq->heap[1].priority = new_priority;
		pq->id_ptrs[new_id] = &(pq->heap[1]);		
	}

	pq->id_ptrs[*id] = NULL;
	pq->size--;

	if(pq_size > 1)
		percolate_down(pq, 1);

	return 1;
}

int pq_capacity(PQ* pq) {
	return pq->capacity;
}

int pq_size(PQ* pq) {
	return pq->size;
}

int pq_contains(PQ* pq, int id) {
	return pq->id_ptrs[id] != NULL;
}


/**
* General description:  priority queue which stores pairs
*   <id, priority>.  Top of queue is determined by priority
*   (min or max depending on configuration).
*
*   There can be only one (or zero) entry for a particular id.
*
*   Capacity is fixed on creation.
*
*   IDs are integers in the range [0..N-1] where N is the capacity
*   of the priority queue set on creation.  Any values outside this
*   range are not valid IDs.
**/



// "Opaque type" -- definition of pq_struct hidden in pq.c
typedef struct pq_struct PQ;


/**
* Function: pq_create
* Parameters: capacity - self-explanatory
*             min_heap - if 1 (really non-zero), then it is a min-heap
*                        if 0, then a max-heap
*                        
* Returns:  Pointer to empty priority queue with given capacity and
*           min/max behavior as specified.
*
*/
extern PQ * pq_create(int capacity, int min_heap);

/**
* Function: pq_free
* Parameters: PQ * pq
* Returns: --
* Desc: deallocates all memory associated with passed priority 
*       queue.
*
*/
extern void pq_free(PQ * pq);

/**
* Function: pq_insert
* Parameters: priority queue pq
*             id of entry to insert
*             priority of entry to insert
* Returns: 1 on success; 0 on failure.
*          fails if id is out of range or
*            there is already an entry for id
*          succeeds otherwise.
*          
* Desc: self-explanatory
*
* Runtime:  O(log n)
*
*/
extern int pq_insert(PQ * pq, int id, double priority);

/**
* Function: pq_change_priority
* Parameters: priority queue ptr pq
*             element id
*             new_priority
* Returns: 1 on success; 0 on failure
* Desc: If there is an entry for the given id, its associated
*       priority is changed to new_priority and the data 
*       structure is modified accordingly.
*       Otherwise, it is a failure (id not in pq or out of range)
* Runtime:  O(log n)
*       
*/
extern int pq_change_priority(PQ * pq, int id, double new_priority);

/**
* Function: pq_remove_by_id
* Parameters: priority queue pq, 
*             element id
* Returns: 1 on success; 0 on failure
* Desc: if there is an entry associated with the given id, it is
*       removed from the priority queue.
*       Otherwise the data structure is unchanged and 0 is returned.
* Runtime:  O(log n)
*
*/
extern int pq_remove_by_id(PQ * pq, int id);

/**
* Function: pq_get_priority
* Parameters: priority queue pq
*             elment id
*             double pointer priority ("out" param)
* Returns: 1 on success; 0 on failure
* Desc: if there is an entry for given id, *priority is assigned 
*       the associated priority and 1 is returned.
*       Otherwise 0 is returned and *priority has no meaning.
* Runtime:  O(1)
*
*/
extern int pq_get_priority(PQ * pq, int id, double *priority);

/**
* Function: pq_delete_top
* Parameters: priority queue pq
*             int pointers id and priority ("out" parameters)
* Returns: 1 on success; 0 on failure (empty priority queue)
* Desc: if queue is non-empty the "top" element is deleted and
*       its id and priority are stored in *id and *priority; 
*       The "top" element will be either min or max (wrt priority)
*       depending on how the priority queue was configured.
*    
*       If queue is empty, 0 is returned.
*
* Runtime:  O(log n)
*
*
*/
extern int pq_delete_top(PQ * pq, int *id, double *priority);

/**
* Function:  pq_capacity
* Parameters: priority queue pq
* Returns: capacity of priority queue (as set on creation)
* Desc: see returns
*
* Runtime:   O(1)
*
*/
extern int pq_capacity(PQ * pq);

/**
* Function: pq_size
* Parameters: priority queue pq
* Returns: number of elements currently in queue
* Desc: see above
*
* Runtime:  O(1)
*/
extern int pq_size(PQ * pq);

/**
* Function: pq_contains
* Parameters: priority queue pq
*             element id
* Returns: 1 if there is an entry in the queue for given id
*          0 otherwise
* Desc: see above
*
* Runtime:  O(1)
*
* Note:  same return value as pq_get_priority
*
*/
extern int pq_contains(PQ * pq, int id);




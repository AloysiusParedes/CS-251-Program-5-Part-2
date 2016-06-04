
typedef unsigned (*HFUNC)(char *);

typedef struct hmap *HMAP_PTR;


#define NAIVE_HFUNC 0
#define BASIC_WEIGHTED_HFUNC 1

#define DEFAULT_HFUNC_ID BASIC_WEIGHTED_HFUNC

#define DEFAULT_INIT_SIZE 128

#define DEFAULT_LFACTOR (0.75)

/**
* Creates and initializes a hash map data structure with
*   given initial table size and specified load factor.
*
* \param init_tsize specifies the desired initial 
*    table size.  If zero is passed, a default table
*    size is used.
*
* \param lfactor specifies the desired maximum load factor;
*    if zero or a negative number is passed, a default 
*    load factor is used.
* \returns HMAP_PTR giving a handle to an initialized 
*    empty hash map.
*/
extern HMAP_PTR hmap_create(unsigned init_tsize, double lfactor);

/**
* \returns number of distinct keys in the map
*/
extern int hmap_size(HMAP_PTR map);

/**
* prints a crude ascii profile of the distribution
*   of keys to table entries.
*   Shows collisions.
*/
extern void hmap_display(HMAP_PTR map); 

/**
* sets hash function as specified by hfunc_id to one
*   of the pre-defined hash functions.
* if table not empty, operation fails and returns 0.
* See #defines for NAIVE_HFUNC and BASIC_WEIGHTED_HFUNC 
*   for example
*/
extern int hmap_set_hfunc(HMAP_PTR map, int hfunc_id); 

/**
* sets hash function to user-specified hfunc if table 
*    empty (and returns 1).
* if table non-empty, hash function is unchanged and
*    zero is returned.
* \param desc is a string descibing the hash function.
*/
extern int hmap_set_user_hfunc(HMAP_PTR map, HFUNC hfunc, char *desc);


/**
* determines if the given key is in the table and returns
*   0/1 accordingly.
* does not do anything with the value field.
*/
extern int hmap_contains(HMAP_PTR map, char *key);


/**
* if given key is in table, corresponding value is returned.
* If not, NULL is returned.
*
* Note:  key could be in the table with NULL as its 
*    corresponding value (as specified by the client).
*    In this case, the caller cannot distinguish between 
*    a NULL value and the key not being in the table.
*
*    hmap_contains should be used in such cases
*/
extern void *hmap_get(HMAP_PTR map, char *key);

/**
* sets the value associated with key to the given value
*   (val).
*
* If key not already in the table, a new entry is
*   created.  (There is no separate insert function)
* \returns previous value associated with key (if any);
*   NULL if key not previously present.
*/
extern void * hmap_set(HMAP_PTR map, char *key, void *val);


/**
* Removes entry corresponding to given key (if any).
* \returns previously associated value (if key not
*    in table, NULL is returned.)
*/
extern void *hmap_remove(HMAP_PTR map, char *key); 


/**
* Prints statistical information about the map.
*
* currently unimplemented
*/
extern void hmap_print_stats(HMAP_PTR map); 

/**
* Deallocates all memory internally allocated for the map
*
* If free_vals_flag is true, the associated values are also
*   deallocated (e.g., to save the client the trouble
*   of deallocating them).  If client did not dynamically
*   allocate values, then free_vals_flag should always be
*   false!
*
* Note:  Values are not deallocated; if client uses
*   dynamic memory allocation for values, it is their
*   responsibility to deallocate them.
*/
extern void hmap_free(HMAP_PTR map, int free_vals_flag); 




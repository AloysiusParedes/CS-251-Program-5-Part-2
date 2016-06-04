#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hmap.h"

#define BASE1 27


/******** STRUCTS AND TYPEDEFS *********/

typedef struct node_struct_hmap {
    struct node_struct_hmap *next;
    char *key;
    void *val;
    unsigned hval;
}NODE_HMAP;

typedef struct {
    NODE_HMAP *members;
    int n;
} TBL_ENTRY;

struct hmap {
    TBL_ENTRY *tbl;
    int tsize;
    int n; 
    double lfactor;
    int max_n;
    HFUNC hfunc;
    char *hfunc_desc;
};

typedef struct {
	HFUNC hfunc;
	char *description;
} HFUNC_STRUCT;

/******** END STRUCTS AND TYPEDEFS *********/

/******** LIBRARY SUPPLIED HASH FUNCTIONS *****/

static unsigned h0(char *s) {
unsigned h = 0;

  while(*s != '\0'){
	h += *s;
	s++;
  }
  return h;
}


static unsigned h1(char *s) {
unsigned h=0;

  while(*s != '\0') {
	h = h*BASE1 + *s;
	s++;
  }
  return h;
}
/******** END LIBRARY SUPPLIED HASH FUNCTIONS *****/



/***** TWO GLOBALS TO THIS FILE ******/
static HFUNC_STRUCT HashFunctions[] = 
	{ 
		{h0, "naive char sum"},
		{h1, "weighted char sum"}
	};


static int NumHFuncs = sizeof(HashFunctions)/sizeof(HFUNC_STRUCT);

/***** END GLOBALS ******/

/***** FORWARD DECLARATIONS *****/
static int match(char *key, unsigned hval, NODE_HMAP *p); 
static NODE_HMAP **get_node_pred(HMAP_PTR map, char *key); 
static TBL_ENTRY *create_tbl_array(int tsize); 
static void resize(HMAP_PTR map); 
static void free_lst(NODE_HMAP *l, int free_vals); 
static void add_front(TBL_ENTRY *entry, NODE_HMAP *p);
/***** END FORWARD DECLARATIONS *****/


/***** BEGIN hmap FUNCTIONS ******/

HMAP_PTR hmap_create(unsigned init_tsize, double lfactor){
HMAP_PTR map = malloc(sizeof(struct hmap));

  map->n = 0;
  if(lfactor <= 0)
	lfactor = DEFAULT_LFACTOR;
  if(init_tsize <= 0)
	init_tsize = DEFAULT_INIT_SIZE;
  
  map->lfactor = lfactor;
  map->tsize = init_tsize;
  map->max_n = init_tsize * lfactor;

  map->hfunc = HashFunctions[DEFAULT_HFUNC_ID].hfunc;
  map->hfunc_desc = HashFunctions[DEFAULT_HFUNC_ID].description;

  map->tbl = create_tbl_array(init_tsize);
 
  return map;
}


int hmap_size(HMAP_PTR map) {
  return map->n;
}

void hmap_display(HMAP_PTR map) {
int i, j;
  
  for(i=0; i<map->tsize; i++) {
      printf("|-|");
      for(j=0; j<map->tbl[i].n; j++) 
	  printf("X");
      printf("\n");
  }
}
int hmap_set_hfunc(HMAP_PTR map, int hfunc_id) {
  if(map->n > 0) {
	fprintf(stderr, 
	  "warning:  attempt to change hash function on non-empty table\n");
	return 0;
  }
  if(hfunc_id < 0 || hfunc_id >= NumHFuncs) {
	fprintf(stderr, 
	  "warning:  invalid hash function id %i\n", hfunc_id);
	return 0;
  }
  map->hfunc = HashFunctions[hfunc_id].hfunc;
  map->hfunc_desc = HashFunctions[hfunc_id].description;
  return 1;
}

int hmap_set_user_hfunc(HMAP_PTR map, HFUNC hfunc, char *desc) {
  if(map->n > 0) {
	fprintf(stderr, 
	  "warning:  attempt to change hash function on non-empty table\n");
	return 0;
  }
  map->hfunc = hfunc;
  if(desc == NULL)
    map->hfunc_desc = "user-supplied hash function";
  else 
    map->hfunc_desc = desc;
  return 1;
}



int hmap_contains(HMAP_PTR map, char *key) {
NODE_HMAP **pp;
  pp = get_node_pred(map, key);
  return (*pp == NULL ? 0 : 1);
}

void *hmap_get(HMAP_PTR map, char *key) {
NODE_HMAP **pp;
  pp = get_node_pred(map, key);
  return (*pp == NULL ? NULL : (*pp)->val);
}

  

	

  

void * hmap_set(HMAP_PTR map, char *key, void *val){
unsigned h;
int idx;
NODE_HMAP *p, **pp;

  pp = get_node_pred(map, key);
  p = *pp;

  if(p == NULL) {  // key not present
     char *key_clone;

     key_clone = malloc( (strlen(key) + 1)*sizeof(char));
     strcpy(key_clone, key);

     map->n++;
     if(map->n > map->max_n) 
	resize(map);
     h = map->hfunc(key);
     idx = h % map->tsize;

     p = malloc(sizeof(NODE_HMAP));

     p->key = key_clone;
     p->val = val;
     p->hval = h;

     add_front(&(map->tbl[idx]), p);
     return NULL;  // no old value to return
  }
  else {  // key already in map
     void *tmp = p->val;
     p->val = val;
     return tmp;  // return old value
  }
}


void *hmap_remove(HMAP_PTR map, char *key) {
NODE_HMAP *p, **pp;

  pp = get_node_pred(map, key);
  p = *pp;
  if(p == NULL){
	return NULL;
  }
  else {
	unsigned idx;
	void *val = p->val;

	*pp = p->next;  // make predecessor skip node
			//   being removed
	free(p->key);
	free(p);

  	idx = (p->hval) % map->tsize;
	map->tbl[idx].n--;
	map->n--;
	return val;
  }
}
  


void hmap_print_stats(HMAP_PTR map) {

    printf("sorry, hmap_print_stats() not implemented\n");

}

void hmap_free(HMAP_PTR map, int free_vals) {
int i;

  for(i=0; i<map->tsize; i++) 
	free_lst(map->tbl[i].members, free_vals);
  free(map->tbl);
  map->tbl = NULL;  // not needed
  free(map);
}



/**** UTILITY FUNCTIONS *******/

static int match(char *key, unsigned hval, NODE_HMAP *p) {
  return (p->hval == hval && strcmp(key, p->key)==0);
}

static NODE_HMAP **get_node_pred(HMAP_PTR map, char *key) {
unsigned h;
int i;
NODE_HMAP **pp;
  h = map->hfunc(key);
  i = h % map->tsize;

  pp =&(map->tbl[i].members); 
  while( *pp != NULL) {
    if(match(key, h, *pp)) 
	return pp;
    pp = &((*pp)->next);
  }
  return pp;
}

static void add_front(TBL_ENTRY *entry, NODE_HMAP *p) {
  entry->n++;
  p->next = entry->members;
  entry->members = p;
}

static TBL_ENTRY *create_tbl_array(int tsize) {
int i;
TBL_ENTRY *tbl;
NODE_HMAP *p;

  tbl = malloc(tsize * sizeof(TBL_ENTRY));
  for(i=0; i<tsize; i++) {
	tbl[i].members = NULL;
	tbl[i].n = 0;
  }
  return tbl;
}

static void resize(HMAP_PTR map) {
int ntsize;
TBL_ENTRY *ntbl;
NODE_HMAP *nxt, *p;
unsigned h;
int i, idx;

  ntsize = 2*map->tsize;
  ntbl = create_tbl_array(ntsize);

  for(i=0; i<map->tsize; i++) {
    p = map->tbl[i].members;
    while(p != NULL) {
	nxt = p->next;
  	// h = map->hfunc(key);  // no need to recompute
  	idx = p->hval % ntsize;
	add_front(&ntbl[idx], p);
	p = nxt;
    }
  }
  free(map->tbl);
  map->tbl = ntbl;
  map->tsize = ntsize;
  map->max_n = (int)(ntsize * map->lfactor);

}
static void free_lst(NODE_HMAP *l, int free_vals) {
  if(l == NULL) return;
  free_lst(l->next, free_vals );
  free(l->key);  // made our own copy
  if(free_vals &&  l->val != NULL)
	free(l->val);
  free(l);
}
/**** END UTILITY FUNCTIONS *******/




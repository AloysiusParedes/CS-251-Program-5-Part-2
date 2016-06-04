#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "pq.h"
#include "hmap.h"

/**** STRUCT ****/
/* struct for node */
typedef struct lst_node {
    int node_id;			//holds id of node
    struct lst_node *next;	//pointer to next node in vertex linked list
	char *nodeName;			//holds the name of the node
	double edge;			//holds the distanct between 2 nodes
} LST_NODE;

/* struct for vertex */
typedef struct {
    int out_degree;			//number of possible destinations from current vertex (number of nodes in linked list)
    LST_NODE *neighbors; 	//array of nodes (head of linked list)
	char *vertexName;		//holds the name of the vertex
	int vertex_id;			//holds the id of the vertex
} VERTEX;

/* struct for graph */
typedef struct graph {
    int size;				//max size of graph (number of vertices)
    VERTEX *vertices;   	//srray of vertices
	int currSize;			//current size of the graph
} GRAPH_PTR;

/**** FUNCTION PROTOTYPES ****/
GRAPH_PTR* graph_build(int n);
void graph_add_edge(GRAPH_PTR* g, int u, int v, double edge, char* name);
void graph_print(GRAPH_PTR* g);
void graph_free(GRAPH_PTR* g);
void graph_insert_vert_name(GRAPH_PTR* g, char *name, int *position);
void graph_print_vertices(GRAPH_PTR* g);
void printDistances(double arr[], int source);
double * dijkstra(GRAPH_PTR* g, int start, int destination, int flag);
int *alloc_arr(int n);

/**** MAIN FUNCTION ****/
int main(int argc, char **argv){
	/** VARIABLE DECLARATIONS **/
	int i;					//for loops baby for loops
	int numVertices = 0;	//to hold number of vertices in graph
	char *start;			//to hold start positions
	char *destination;		//to hold destination positions
	char *distanceString;	//to hold string version of edge length
	int flag = 0;			//flag to fix first line of file being read in
	double edgeLength;		//to hold value of edge length
	size_t line_size = 11;	//for reading in the file
	int *dijkstraVal;		//to hold value from hmap to use in dijkstra
	double *distVals;		//to hold array of distances from start vertices
	int userMove;			//to hold users possible moves (either 0, 1, 2... (possible moves))
	double optimalDistance;	//to hold the shortest distance
	double minDistance;		//to hold the minimum distance to destination
	int sourcePosition;		//to hold start position
	int destinationPosition;//to hold destination postiion
	
	int **a = malloc(sizeof(int*) * 10000);		//allocate space for alloc_arr function
	int **b = malloc(sizeof(int*) * 10000);		//allocate space for alloc_arr function
	start = malloc(sizeof(char) * 11);			//allocate space for user input start string
	destination = malloc(sizeof(char) * 11);	//allocate space for user input destination string
	distanceString = malloc(sizeof(char) * 11);	//allocate space for user input distance string
	
	//print header
	printf("\n\tWelcome to travel planner.\n\n");
	
	/** open and read file **/
	//check to see if there is a file 
	if(argv[1] == NULL){
		printf("\n\tERROR: Can't open file\n");
		free(a);				//free alloc_arr variable
		free(b);				//free alloc_arr variable
		free(start);			//free the start position
		free(destination);		//free the destination position
		free(distanceString);	//free the string that holds the distance
		return 1;
	}
	
	FILE *f = fopen(argv[1], "r"); 					// "r" for read
	char *buff = malloc(sizeof(char) * line_size);	//a buffer to hold what is read in
	
	//read in first line, print error if not able to
	if(fscanf(f, "%d", &numVertices) == 0){
		printf("\n\tERROR: Can't read in first integer\n");
		free(a);				//free alloc_arr variable
		free(b);				//free alloc_arr variable
		free(buff);				//free the buffer
		free(start);			//free the start position
		free(destination);		//free the destination position
		free(distanceString);	//free the string that holds the distance
		return 1;
	}
	
	//initialize graph
	GRAPH_PTR *graph = graph_build(numVertices);
	//initialize hmap
	HMAP_PTR map = hmap_create(0, 1.0);
	int arr_alloc_counter = 0;	//used for alloc_arr func
	//read in the rest of the file
	while(getline(&buff, &line_size, f) != -1){
		//skip the first line of file (read in previously)
		if(flag == 0){
			flag++;
			continue;
		}
		int counter = 1;		//keeps track of which token goes to what string
		
		//loop the rest of the line and tokenize
		char *string = strtok(buff, " ");
		while(string != NULL){
			//read in start value
			if(counter == 1){
				strcpy(start, string);
				counter++;
			}
			//read in destination value
			else if(counter == 2){
				strcpy(destination, string);
				counter++;
			}
			//read in distance value
			else{
				strcpy(distanceString, string);
				counter++;
			}
			//move to next white-space separated word
			string = strtok(NULL, " ");
		}
		//reset counter for next line
		counter = 0;
		//parse the distanceString as a double
		edgeLength = strtod(distanceString, NULL);
		
		/* store values */
		//store vertex name into graph
		graph_insert_vert_name(graph, start, &sourcePosition);
		//store vertex name into hmap
		//set alloc_arr for hmap_set
		a[arr_alloc_counter] = alloc_arr(1);
		hmap_set(map, start, a[arr_alloc_counter]);
		dijkstraVal = hmap_get(map, start);
		dijkstraVal[0] = sourcePosition;
		
		/* store values */
		//store vertex name into graph
		graph_insert_vert_name(graph, destination, &destinationPosition);
		//store vertex name into hmap
		//set alloc_arr for hmap_set
		b[arr_alloc_counter] = alloc_arr(1);
		hmap_set(map, destination, b[arr_alloc_counter]);
		dijkstraVal = hmap_get(map, destination);
		dijkstraVal[0] = destinationPosition;
		
		//store neighbors
		graph_add_edge(graph, sourcePosition, destinationPosition, edgeLength, destination);
		graph_add_edge(graph, destinationPosition, sourcePosition, edgeLength, start);
		arr_alloc_counter++;
	}
	//close the file that was opened
	fclose(f);
	
	//print a list of all the vertices in the graph by name
	graph_print_vertices(graph);
	
	//ask the use for their current location which the user enters as a vertex name
	printf("\nSELECT YOUR CURRENT LOCATION:\t");
	scanf("%s", start);
	
	//hmap the start location
	dijkstraVal = hmap_get(map, start);
	//check to see if vertex is in the hmap
	if(dijkstraVal == NULL){
		printf("\nVertex does not exist\n\n");
		int i;
		/* free allocated memory */
		for (i = 0; i < arr_alloc_counter; i++){
			free(a[i]);
			free(b[i]);
		}
		free(a);				//free alloc_arr variable
		free(b);				//free alloc_arr variable
		free(buff);				//free the buffer
		free(start);			//free the start position
		free(destination);		//free the destination position
		free(distanceString);	//free the string that holds the distance
		graph_free(graph);		//free the graph
		hmap_free(map, 0);		//free hmap
		return 1;
	}
	
	//run dijkstra's algorithm
	distVals = dijkstra(graph, dijkstraVal[0], destinationPosition, 0);
		
	//ask the user for their destination (also specified by vertex name).
	printf("SELECT YOUR DESTINATION     :\t");
	scanf("%s", destination);
	
	//hmap the destination location
	dijkstraVal = hmap_get(map, destination);
	//check to see if vertex is in the hmap
	if(dijkstraVal == NULL){
		printf("\nVertex does not exist\n\n");
		int i;
		/* free allocated memory */
		for (i = 0; i < arr_alloc_counter; i++){
			free(a[i]);
			free(b[i]);
		}
		free(a);				//free alloc_arr variable
		free(b);				//free alloc_arr variable
		free(buff);				//free the buffer
		free(start);			//free the start position
		free(destination);		//free the destination position
		free(distanceString);	//free the string that holds the distance
		graph_free(graph);		//free the graph
		hmap_free(map, 0);		//free hmap
		return 1;
	}
	
	//check to see if destination is unreachable
	if(distVals[dijkstraVal[0]] == INT_MAX){
		printf("\nCannot go from %s to %s\n\n", start, destination);
		int i;
		/* free allocated memory */
		for (i = 0; i < arr_alloc_counter; i++){
			free(a[i]);
			free(b[i]);
		}
		free(a);				//free alloc_arr variable
		free(b);				//free alloc_arr variable
		free(buff);				//free the buffer
		free(start);			//free the start position
		free(destination);		//free the destination position
		free(distanceString);	//free the string that holds the distance
		graph_free(graph);		//free the graph
		hmap_free(map, 0);		//free hmap
		return 1;
		return;
	}
	//print out the shortest distance to that destination
	else{
		optimalDistance = distVals[dijkstraVal[0]];
		printf("\nYou can reach your destination in %.2lf units.\n\n", optimalDistance);
	}	
	
	dijkstraVal = hmap_get(map, start);
	//set current destination to start location
	int currLoc = dijkstraVal[0];
	dijkstraVal = hmap_get(map, destination);
	//set destination location to destination location
	int destLoc = dijkstraVal[0];
	double totalDistanceTraveled = 0.0;
	minDistance = optimalDistance;
	free(distVals);
	
	//print out shortest path
	distVals = dijkstra(graph, currLoc, destLoc, 1);
	free(distVals);
	/** interactive loop **/
	printf("\nTravel Time!\n\n");
	//loop while current location != destination location
	while(currLoc != destLoc){
		int j = 1;
		//print out total distance traveled so far
		printf("\tTOTAL DISTANCE TRAVELED:\t%.2lf\n", totalDistanceTraveled);
		//print out current location
		printf("\tCURRENT LOCATION:\t\t%s\n", graph->vertices[currLoc].vertexName);
		//print out destination location
		printf("\tDESTINATION LOCATION:\t\t%s\n", graph->vertices[destLoc].vertexName);
		//print the minimum distance from user's current location to destination
		printf("\tMIN DISTANCE TO DESTINATION:\t%.2lf\n", minDistance);
		
		//list user options for their next move: give up or select one of the vertices directly connected to the current location
		printf("\n\tPOSSIBLE MOVES:\n\t\t0. I give up!\n");
		//temp variable to traverse the neighbors linked list
		LST_NODE *temp = graph->vertices[currLoc].neighbors;
		//loop through the neighbors
		while(temp != NULL){
			printf("\t\t%d. %s\t(%.2lf)\n", j, temp->nodeName, temp->edge);
			temp = temp->next;
			j++;
		}
		
		//TODO give recommended move (i.e. vertex on a shortest path to destination)
		printf("\tRECOMMENDED MOVE: %s\n", graph->vertices[currLoc].neighbors->nodeName);
		
		userMove = -1;
		//reads the user selection as an integer until it reads a valid user move
		while(1){
			printf("\tSELECT A MOVE (ENTER A NUMBER): ");
			scanf("%d", &userMove);
			if(userMove < 0 || userMove >= j)
				printf("Invalid move. Try again\n");
			else
				break;
		}
		
		//check if the user gave up
		if(userMove == 0){
			printf("\nThank you for traveling!\nGoodbye!\n\n");
			for (i = 0; i < arr_alloc_counter; i++){
				free(a[i]);
				free(b[i]);
			}
			free(a);				//free alloc_arr variable
			free(b);				//free alloc_arr variable
			free(buff);				//free the buffer
			free(start);			//free the start position
			free(destination);		//free the destination position
			free(distanceString);	//free the string that holds the distance
			graph_free(graph);		//free the graph
			hmap_free(map, 0);		//free hmap
			return 1;
		}
		//user wishes to travel to a neighbor
		else{ 
			int traverse = 1;							//variable to keep track of where you are in the neighbors linked list
			temp = graph->vertices[currLoc].neighbors;	//temp variable to traverse the neighbors
			//loop until you've reached the neighbor the user wishes to travel to
			while(traverse != userMove){
				temp = temp->next;
				traverse++;
			}
			//total distance traveled is updated
			totalDistanceTraveled += temp->edge;
			//user's current location is updated
			currLoc = temp->node_id;
			//min distance to destination is updated
			dijkstraVal = hmap_get(map, temp->nodeName);
			distVals = dijkstra(graph, dijkstraVal[0], destinationPosition, 0);
			dijkstraVal = hmap_get(map, destination);
			minDistance = distVals[dijkstraVal[0]];
			free(distVals);
		}
		printf("==============================================\n\n");
	}
	
	//broke out up while loop means currLoc == destLoc
	printf("YOU MADE IT.\n");
	printf("Total Distance: %.2lf\n", totalDistanceTraveled);
	printf("Optimal Distance: %.2lf\nGoodbye\n\n", optimalDistance);
	
	/* free allocated memory */
	for (i = 0; i < arr_alloc_counter; i++){
		free(a[i]);
		free(b[i]);
	}
	free(a);				//free alloc_arr variable
	free(b);				//free alloc_arr variable
	free(buff);				//free the buffer
	free(start);			//free the start position
	free(destination);		//free the destination position
	free(distanceString);	//free the string that holds the distance
	graph_free(graph);		//free the graph
	hmap_free(map, 0);		//free hmap
	return 0;
}//end main(...)

/**** FUNCTION DEFINITIONS ****/
/* create a graph of size n */
GRAPH_PTR* graph_build(int n){
    GRAPH_PTR *g = malloc(sizeof(struct graph));
    g->size = n;
    g->vertices = malloc(n * sizeof(VERTEX));
	g->currSize = 0;	//initialize starting size as 0
    int i = 0;
    for(i = 0; i < n; i++) {
        g->vertices[i].out_degree = 0;
        g->vertices[i].neighbors = NULL;
		g->vertices[i].vertexName = NULL;
		g->vertices[i].vertex_id = -1;
    }
    return g;
}//end graph_build(...)

/* insert edge to vertex */
void graph_add_edge(GRAPH_PTR* g, int u, int v, double edge, char* name){
    LST_NODE *new = malloc(sizeof(LST_NODE));		//allocate a new node
    new->node_id = v;						//set node's id to v
	
	new->edge = edge;
	//new->nodeName = malloc(sizeof(char) * 15);
	new->nodeName = g->vertices[v].vertexName;
	
    new->next = g->vertices[u].neighbors;	//set the next pointer to point to the graph's front node
    g->vertices[u].neighbors = new; 		//set to new node to be the graph's front node
    g->vertices[u].out_degree++;			//increment number of possible destinations from that vertex of the graph
}//end graph_add_edge(...)

/* print the graph */
void graph_print(GRAPH_PTR* g){
    int i;
    for(i = 0; i < g->size; i++) {
        printf("[%d", i);
		//print linked list
        LST_NODE *cur = g->vertices[i].neighbors;
		if(g->vertices[i].vertexName != NULL)
			printf("-%s]: ", g->vertices[i].vertexName);
		else
			printf("]: ");
		
		if(cur == NULL)
			printf("EMPTY");
		else
			while(cur != NULL) {
				printf("(%d-%s, %.2lf), ", cur->node_id, cur->nodeName, cur->edge);
				cur = cur->next;
			}
        printf("\n");
    }
}//end graph_print(...)

/* deallocate space created by graph_build */
void graph_free(GRAPH_PTR* g){
    int i = 0;
    for(i = 0; i < g->size; i++){
        LST_NODE *cur = g->vertices[i].neighbors;
        while(cur != NULL) {
            LST_NODE *temp = cur->next;
            free(cur);
            cur = temp;
        }
		free(g->vertices[i].vertexName);
    }	
    free(g->vertices);
    free(g);
}//end graph_print(...)

/* insert vertex names */
void graph_insert_vert_name(GRAPH_PTR* g, char *name, int *position){
	int i;
	//check if graph has no vertices
	if(g->currSize == 0){
		g->vertices[g->currSize].vertexName = malloc(sizeof(char) * 11);
		strcpy(g->vertices[g->currSize].vertexName, name);
		g->vertices[g->currSize].vertex_id = g->currSize;
		
		*position = g->currSize;
		
		//check to see if size will go over
		if(g->currSize + 1 > g->size)
			exit(1);
		g->currSize++;
	}
	//loop through all existing vertices
	for(i = 0; i < g->currSize; i++)
		//if vertex already exists, do nothing
		if(strcmp(g->vertices[i].vertexName, name) == 0){
			*position = i;
			return;		
		}
	
	//otherwise, add new vertex at currSize index
	g->vertices[g->currSize].vertexName = malloc(sizeof(char) * 11);
	strcpy(g->vertices[g->currSize].vertexName, name);
	g->vertices[g->currSize].vertex_id = g->currSize;
	
	*position = g->currSize;
	
	//check to see if size will go over
	if(g->currSize + 1 > g->size)
			exit(1);
	g->currSize++;
	return;
}

/* print the vertices of the graph */
void graph_print_vertices(GRAPH_PTR* g){
	printf("Vertices:\n   ");
	//check if the current size is 0
	if(g->size == 0){
		printf("ERROR: No vertices in graph!\n");
		exit(1);
	}
	//loop through existing vertices
	int i;
	for(i = 0; i < g->size; i++)
		if(g->vertices[i].vertexName != NULL)
			printf("%s ", g->vertices[i].vertexName);
	printf("\n");
}

/* dijkstra's algroithm to find the shortest path from start position to all vertices */
double * dijkstra(GRAPH_PTR* g, int start, int destination, int flag){
	int i, j;						//for loops baby for loops
	int numVertices = g->currSize;	//hold the current number of vertices
	double *distVals;				//hold distance values
	double topValue;				//holds the top value of the heap. (min of heap)
	int vertexNumber;				//holds the vertex number
	int visited[numVertices];		//holds visted vertices
	int pred[numVertices];			//holds previous nodes
	
	//allocate space for distance array
	distVals = malloc(sizeof(double) * numVertices);
	//create min heap
	PQ *minHeap = pq_create(numVertices, 1);

	//initialize min-heap with all vertices and distance values of all vertices
	for(i = 0; i < numVertices; ++i){
		//initialize to max of int
		distVals[i] = INT_MAX;
		visited[i] = 0;
		pq_insert(minHeap, i, distVals[i]);
	}
	
	//make distance values of start vertex as 0 so it is extracted first
	distVals[start] = 0.0;
	//mark position as visited
	visited[start] = 1;
	//set previous node
	pred[start] = start;
	//change priority
	pq_change_priority(minHeap, start, distVals[start]);
	
	//loop to finalize shortest distance
	
	while(pq_size(minHeap) > 0){
		//extract vertex number and value at the top of the heap
		pq_delete_top(minHeap, &vertexNumber, &topValue);
		//mark that vertex visited
		visited[vertexNumber] = 1;
		//temp node for traversal
		LST_NODE *temp = g->vertices[vertexNumber].neighbors;
		
		//loop through all adjacent vertices
		while(temp != NULL){
			i = temp->node_id;
			
			//check if vertex is visited and shortest distance to i is not finalized yet, and distance to i through vertexNumber is less than it's previously calculated distance
			if(pq_contains(minHeap, i) && distVals[vertexNumber] != INT_MAX && (temp->edge + distVals[vertexNumber]) < distVals[i]){
				//check visited status
				if(!visited[i])
					//set new previous node
					pred[i] = vertexNumber;
				//update the distance value at that vertex
				distVals[i] = distVals[vertexNumber] + temp->edge;
				//change priorities
				pq_change_priority(minHeap, i, distVals[i]);
			}
			//move to next node
			temp = temp->next;
		}
	}
	if(flag == 1){
		//print the shortest path
		j = pred[destination];
		
		//array to hold path names
		char **path = malloc(sizeof(char*) * g->currSize);
		for(i = 0; i < g->currSize; i++){
			path[i] = malloc(sizeof(char) * 11);
			strcpy(path[i], "null");
		}
		
		i = 0;
		//loop to copy path taken to path array
		do{
			j = pred[j];
			//printf("\t%s\n", g->vertices[j].vertexName);
			strcpy(path[i], g->vertices[j].vertexName);
			i++;
		}
		while(j != start);
		
		printf("SHORTEST PATH:\n");
		//print the path in reverse
		for(i = g->currSize-1; i >= 0; i--){
			if(strcmp(path[i], "null") == 0)
				continue;
			else 
				printf("\t%s ->\n", path[i]);
		} 
		
		printf("\t%s\n", g->vertices[destination].vertexName);
		for(i = 0; i < g->size; i++)
			free(path[i]);
		free(path);
	}
	
	pq_free(minHeap);
	return distVals;
}

/* prints distances from source */
void printDistances(double arr[], int source){
	int i;
	printf("\nVertex\tDistance From Source\n");
	for(i = 0; i < source; ++i)
		printf("%d\t\t%.2lf\n", i, arr[i]);
}

/* used for hmap */
int *alloc_arr(int n){
	int i;
	int *a;
	a = malloc(n * sizeof(int));
	for(i=0; i<n; i++)
		a[i] = 0;
	
	return a;
}


clean:
	rm hmap.o pq.o graph.o

hmap.o:  hmap.c hmap.h
	gcc -c hmap.c

ssum:  ssum.c hmap.o
	gcc -o ssum ssum.c hmap.o

demo:  demo.c hmap.o
	gcc -o demo demo.c hmap.o

pq.o: pq.c pq.h
	gcc -c pq.c

graph.o: graph.c graph.h
	gcc -c graph.c
	
test: test.c pq.o
	gcc test.c pq.o -o test
	
travel: travel.c pq.o hmap.o
	gcc -g travel.c pq.o hmap.o -o travel
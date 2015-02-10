SUBTOUROBJS= subtour_hw.o lp.o util.o graph.o tnode.o
CPLEX_LIB = /software/cplex/distribution/cplex/lib/x86-64_linux/static_pic/libcplex.a
CPLEX_INCLUDE = /software/cplex/distribution/cplex/include/ilcplex

CC= g++
CPPFLAGS=  -O3 -g -Wall -pedantic -Wno-long-long -I$(CPLEX_INCLUDE)


subtour: $(SUBTOUROBJS)
	  $(CC) $(CPPFLAGS) -o subtour $(SUBTOUROBJS) $(CPLEX_LIB) -lm -lpthread

test_graph: test_graph.o graph.o tnode.o
	  $(CC) $(CPPFLAGS) -o test_graph test_graph.o graph.o tnode.o -lm -lpthread

clean:
	rm -f *.o subtour test_graph

subtour_hw.o: subtour_hw.cpp util.h graph.h
lp.o: lp.c lp.h
util.o: util.c util.h
graph.o: graph.cpp graph.h tnode.h
tnode.o: tnode.cpp tnode.h
test_graph.o: test_graph.cpp graph.h tnode.h

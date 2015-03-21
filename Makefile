SUBTOUROBJS= tour.o lp.o util.o graph.o tnode.o bhk.o bhk2.o bhk3.o onetree.o edge.o 
CPLEX_LIB = /software/cplex/distribution/cplex/lib/x86-64_linux/static_pic/libcplex.a
CPLEX_INCLUDE = /software/cplex/distribution/cplex/include/ilcplex

CC= g++
CPPFLAGS=  -O3 -g -Wall -pedantic -Wno-long-long -I$(CPLEX_INCLUDE)


tour: $(SUBTOUROBJS)
	  $(CC) $(CPPFLAGS) -o tour $(SUBTOUROBJS) $(CPLEX_LIB) -lm -lpthread

test_tour: test_tour.o bhk.o bhk2.o bhk3.o
	  $(CC) $(CPPFLAGS) -o test_tour test_tour.o bhk.o bhk2.o bhk3.o -lm -lpthread

test_graph: test_graph.o graph.o tnode.o
	  $(CC) $(CPPFLAGS) -o test_graph test_graph.o graph.o tnode.o -lm -lpthread

package:
	tar -czf cplex.tar.gz /software
	rm -rf logfile*
	tar -czf /tmp/tsp.tar.gz .
	rm cplex.tar.gz

clean:
	rm -f *.o tour test_graph test_tour

tour.o: tour.cpp util.h graph.h onetree.h
lp.o: lp.c lp.h
util.o: util.c util.h
edge.o: edge.cpp edge.h
graph.o: graph.cpp graph.h tnode.h edge.h
tnode.o: tnode.cpp tnode.h
test_graph.o: test_graph.cpp graph.h tnode.h
bhk.o: bhk.cpp bhk.h
bhk2.o: bhk2.cpp bhk2.h
bhk3.o: bhk3.cpp bhk3.h
test_tour.o: test_tour.cpp bhk.h bhk2.h bhk3.h
onetree.o: onetree.cpp onetree.h

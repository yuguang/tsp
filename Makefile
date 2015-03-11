SUBTOUROBJS= tour.o lp.o util.o graph.o tnode.o bhk.o bhk2.o
CPLEX_LIB = /software/cplex/distribution/cplex/lib/x86-64_linux/static_pic/libcplex.a
CPLEX_INCLUDE = /software/cplex/distribution/cplex/include/ilcplex

CC= g++
CPPFLAGS=  -O3 -g -Wall -pedantic -Wno-long-long -I$(CPLEX_INCLUDE)


tour: $(SUBTOUROBJS)
	  $(CC) $(CPPFLAGS) -o tour $(SUBTOUROBJS) $(CPLEX_LIB) -lm -lpthread

test_tour: test_tour.o bhk.o bhk2.o
	  $(CC) $(CPPFLAGS) -o test_tour test_tour.o bhk.o bhk2.o -lm -lpthread

test_graph: test_graph.o graph.o tnode.o
	  $(CC) $(CPPFLAGS) -o test_graph test_graph.o graph.o tnode.o -lm -lpthread

clean:
	rm -f *.o tour test_graph test_tour

tour.o: tour.cpp util.h graph.h
lp.o: lp.c lp.h
util.o: util.c util.h
graph.o: graph.cpp graph.h tnode.h
tnode.o: tnode.cpp tnode.h
test_graph.o: test_graph.cpp graph.h tnode.h
bhk.o: bhk.cpp bhk.h
bhk2.o: bhk2.cpp bhk2.h
test_tour.o: test_tour.cpp bhk.h bhk2.h


#ifndef bhk2_H
#define bhk2_H

#include <map>

// This class implements the Bellman-Held-Karp algorithm
// Code is mostly copied from the dynamic programming chaper
class bhk2 {

	private:
		int ncount; // number of nodes
		int ecount; // number of edges
		int *elist; // list of edges
		int *elen; // length of each edge

		// stores the given distances from elen
		std::map<std::pair<int,int>, int> distance;

		// returns the distance between two cities
		int getDistance(int city1, int city2);
		
		typedef unsigned int Set;
		int** opt;
		int solve(Set S, int t);

	public:

		bhk2();
		~bhk2();

		void init(int ncount, int ecount, int *elist, int *elen);

		// returns the length of the shortest tour
		int solve();

		// TODO: do we need the actual solution or just the optimal length?

};

#endif

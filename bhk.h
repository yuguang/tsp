#ifndef bhk_H
#define bhk_H

#include <map>
#include <vector>

// This class implements the Bellman-Held-Karp algorithm
class bhk {

	private:
		int ncount; // number of nodes
		int ecount; // number of edges
		int *elist; // list of edges
		int *elen; // length of each edge

		// stores the given distances from elen
		std::map<std::pair<int,int>, int> distance;

		// returns the distance between two cities
		int getDistance(int city1, int city2);

		// function to help generate subsets
		std::vector<std::vector<int> > getSubsets(int size, int end);
		
		// prints out a value in the dynamic programming table (for debugging)
		void printDPval(std::vector<int> subset, int endcity, int value);

	public:

		bhk();
		~bhk();

		void init(int ncount, int ecount, int *elist, int *elen);

		// returns the length of the shortest tour
		int solve();

		// TODO: do we need the actual solution or just the optimal length?

};

#endif

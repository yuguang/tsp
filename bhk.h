#ifndef bhk_H
#define bhk_H

#include <map>

// This class implements the Bellman-Held-Karp algorithm
class bhk {

	private:
		int ncount; // number of nodes
		int ecount; // number of edges
		int *elist; // list of edges
		int *elen; // length of each edge

		// returns the distance between two cities
		int getDistance(int city1, int city2);

		// stores the given distances from elen
		std::map<std::pair<int,int>, int> distance;

	public:

		bhk();
		~bhk();

		void init(int ncount, int ecount, int *elist, int *elen);

		double solve();

		// TODO: do we need the actual solution or just the optimal length?

};

#endif

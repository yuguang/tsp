#ifndef bhk_H
#define bhk_H

#include <map>
#include <vector>

// This class implements the Bellman-Held-Karp algorithm
class bhk {

	private:
		int ncount; // number of nodes
		int **dmatrix; // distances between edges

		// function to help generate subsets
		std::vector<unsigned long long> getSubsets(int size, int end);
		
		// prints out a value in the dynamic programming table (for debugging)
		void printDPval(unsigned long long subset, int endcity, int value);

	public:

		bhk();
		~bhk();

		void init(int ncount, int **dmatrix);

		// returns the length of the shortest tour
		int solve();

		// TODO: do we need the actual solution or just the optimal length?

};

#endif

#ifndef bhk3_H
#define bhk3_H

// This class implements the Bellman-Held-Karp algorithm
// Code is mostly copied from tour_dp3.c
class bhk3 {

	private:
		int ncount; // number of nodes
		int **dmatrix; // distances between edges
		
		int **binomial;
		/*static*/ int *S;
		int *Sbase;
		
		void init_binomial();
		int dist(int i, int j);
		
		typedef int* Set;
		
		void build_S();
		
		unsigned long int Sval(int sz, Set s, int t_indx);
		void firstset(int sz, Set s);
		void nextset(int sz, Set s);

	public:

		bhk3();
		~bhk3();

		void init(int ncount, int **dmatrix);

		// returns the length of the shortest tour
		int solve();

		// TODO: do we need the actual solution or just the optimal length?

};

#endif

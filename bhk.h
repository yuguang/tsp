#ifndef bhk_H
#define bhk_H

// This class implements the Bellman-Held-Karp algorithm
class bhk {

	private:

	public:

		bhk();
		~bhk();

		void init(int ncount, int ecount, int *elist);

		double solve();

		// TODO: do we need the actual solution or just the optimal length?

};

#endif

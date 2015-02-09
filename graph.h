#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "tnode.h"

class graph {
	private:
		tnode* nodes;
                int ncount;
                int ecount;
                int *elist;

	public:
		graph();

		void init(double *x, int ncount, int ecount, int *elist);

		~graph();

                bool is_connected();

                std::vector<std::vector<int> > get_components();

                // returns a list of the indices of edges in delta(s)
                std::vector<int> delta(std::vector<int> s);

};

#endif

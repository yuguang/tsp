#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "tnode.h"
#include <algorithm>

class graph {
	private:
		tnode* nodes;
                int ncount;
                int ecount;
                int *elist;
                int *elen;

	public:
		graph();

		void init(double *x, int ncount, int ecount, int *elist, int *elen);
                void init(int ncount, int ecount, int *elist, int *elen);

		~graph();

                bool is_connected();

                std::vector<std::vector<int> > get_components();

                // returns a list of the indices of edges in delta(s)
                std::vector<int> delta(std::vector<int> s);

                // Computes the MST for the graph
                std::vector<int> min_spanning_tree(std::vector<int> must_include = std::vector<int>(), std::vector<int> must_ignore = std::vector<int>());
};

#endif

#ifndef ONETREE_H
#define ONETREE_H

#include <vector>
#include <set>

class branch_node {
public:
	std::set<int> X, Y;
	int * pi;
	int w;

	branch_node(std::set<int> X, std::set<int> Y, int ncount, int *pi, int w);
	~branch_node();
};

bool operator<(const branch_node & l, const branch_node & r);

int one_tree_tsp(int ncount, int ecount, int *elist, int *elen, int upper_bound);
int w(int ncount, int ecount, int *elist, int *elen, std::set<int> X, std::set<int> Y, int * pi, bool update_pi, std::vector<int> * deg_not_2);
int w_candidate(int ncount, int ecount, int *elist, int *elen, int ignore, int * pi, int * v, std::set<int> X, std::set<int> Y);

#endif
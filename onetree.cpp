#include "onetree.h"
#include "edge.h"
#include "graph.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <set>
#include <queue>

const int t_bar = 1; // Value used to adjust pi
const int p = 15; // If maximum lower bound has not changed in p iterations, stop

// Constructor for a node in our branching tree
branch_node::branch_node(std::set<int> X, std::set<int> Y, int ncount, int *pi, int w) {
	this->X = X;
	this->Y = Y;
	this->pi = new int[ncount];
	memcpy(this->pi, pi, ncount*sizeof(int));
	this->w = w;
	this->ncount = ncount;
}

// Copy constructor
branch_node::branch_node(const branch_node& other) {
	this->X = other.X;
	this->Y = other.Y;
	this->pi = new int[other.ncount];
	memcpy(this->pi, other.pi, other.ncount*sizeof(int));
	this->w = other.w;
	this->ncount = other.ncount;
}

// Assignment Operator
branch_node & branch_node::operator=(const branch_node& other) {
		this->X = other.X;
		this->Y = other.Y;
		memcpy(this->pi, other.pi, other.ncount*sizeof(int));
		this->w = other.w;
		this->ncount = other.ncount;

		return (*this);
}

// Destructor
branch_node::~branch_node() {
	if( pi ) {
		delete [] pi;
	}
}

// Comparison operator for the priority queue
bool operator<(const branch_node & l, const branch_node & r) {
	return l.w > r.w; // Because we want a min heap we flip the sign
}

int one_tree_tsp(int ncount, int ecount, int *elist, int *elen, int upper_bound) {
	int branches = 0, bound, max_w = -100000, max_w_p_ago = -100000, last, i, j, edge;
	int *pi = new int[ncount], *pi_prime = new int[ncount];
	bool do_branch, found_branch;
	std::priority_queue<branch_node> Q;
	std::vector<int> edges, w_cache, deg_not_2, deg_not_2_prime;
	std::set<int> X, Y, X_new, Y_new;

	for( i = 0; i < ncount; i++ ) pi[i] = 0;
 
	w_cache.reserve(p);

	// Find starting lower bound
	bound = w(ncount, ecount, elist, elen, &X, &Y, pi, false, &deg_not_2, &edges);

	// Push to priority Q
	branch_node start = branch_node(X,Y,ncount,pi,bound);
	Q.push(start);

	while( Q.size() != 0 ) {
		// Get branch node with smallest current lower bound
		branch_node current = Q.top();
		bound = current.w;	

		if( bound > upper_bound ) {
			Q.pop(); branches++;
			continue;
		}

		memcpy(pi, current.pi, ncount*sizeof(int));
		X = current.X;
		Y = current.Y;

		Q.pop(); branches++;

		std::set<int>::iterator it;
		// Debug prints
		// std::cout << "Popped (" << current.w << "), branches: " << branches << ", Q size: " << Q.size() << std::endl;
		// std::cout << "pi: "; for( int i = 0; i < ncount; i++ ) std::cout << pi[i] << " "; std::cout << std::endl;
		// std::cout << "X: "; for( it = X.begin(); it != X.end(); it++ ) std::cout << (*it) << " "; std::cout << std::endl;
		// std::cout << "Y: "; for( it = Y.begin(); it != Y.end(); it++ ) std::cout << (*it) << " "; std::cout << std::endl;

		do_branch = true;
		max_w = -100000;
		max_w_p_ago = -100000;
		deg_not_2.erase(deg_not_2.begin(),deg_not_2.end());

		bound = w(ncount, ecount, elist, elen, &X, &Y, pi, false, &deg_not_2, &edges);

		// If popped branch node is a tour, then it is optimal
		if( deg_not_2.size() == 0 ) {
			last = elist[2*edges[0]];
			std::cout << "One Tree Tour: " << last;
			edges.erase(edges.begin(),edges.begin()+1);
			for( j = 0; j < ncount-1; j++ ) {
				for( i = 0; i < (int)edges.size(); i++ ) {
					edge = edges[i];
					if( elist[2*edge] == last ) {
						last = elist[2*edge+1];
						std::cout << " " << last;
						break;
					} else if( elist[2*edge+1] == last ) {
						last = elist[2*edge];
						std::cout << " " << last;
						break;
					}
				}
				edges.erase(edges.begin()+i,edges.begin()+i+1);
			}
			std::cout << std::endl << "Length: " << bound << std::endl;

			delete [] pi; pi = 0;
			delete [] pi_prime; pi_prime = 0;
			return bound;
		}

		// Otherwise, we iterate pi to increase the lower bound
		for( i = 0; ; i++) {
			bound = w(ncount, ecount, elist, elen, &X, &Y, pi, true, &deg_not_2, &edges);

			if( i >= p ) {
				if( w_cache[i%p] >= max_w_p_ago ) {
					max_w_p_ago = w_cache[i%p];
				}
			}
			w_cache[i%p] = bound;
			if( bound >= max_w ) {
				max_w = bound;
				memcpy(pi_prime, pi, ncount*sizeof(int));
				deg_not_2_prime = deg_not_2;
			}
			if( bound >= upper_bound ) {
				do_branch = false;
				break;
			}
			if( max_w_p_ago == max_w ) {
				break;
			}
		}

		// If all edges degree 2, we have a tour, but we don't know it is optimal until it is popped from queue.
		if( deg_not_2_prime.size() == 0 ) {	
			bound = w(ncount, ecount, elist, elen, &X, &Y, pi_prime, false, &deg_not_2, &edges);
			upper_bound = bound; // We can update upper bound though
			Q.push(branch_node(X,Y,ncount,pi_prime,bound));
		}
		if( do_branch ) {
			found_branch = false;
			// We choose a node that does not have degree 2, and branch by rforcing it in/out of the 1-trees
			for( i = 0; i < (int)deg_not_2_prime.size(); i++ ) {
				for( j = 0; j < ecount; j++ ) {
					if( (elist[2*j] == deg_not_2_prime[i] || elist[2*j+1] == deg_not_2_prime[i]) && X.find(j) == X.end() && Y.find(j) == Y.end() ) {
						X_new = X;
						Y_new = Y;
						X_new.insert(j);
						Y_new.insert(j);
						bound = w(ncount, ecount, elist, elen, &X_new, &Y, pi_prime, false, &deg_not_2, &edges);
						Q.push(branch_node(X_new,Y,ncount,pi_prime,bound));
						bound = w(ncount, ecount, elist, elen, &X, &Y_new, pi_prime, false, &deg_not_2, &edges);
						Q.push(branch_node(X,Y_new,ncount,pi_prime,bound));
						found_branch = true;
						break;
					}
				}
				if( found_branch ) {
					break;
				}
			}
		}

	}

	delete [] pi;
	delete [] pi_prime;
	return upper_bound;
}

// Computes the minimum cost 1-tree among the given nodes with edge weights modified by pi
int w(int ncount, int ecount, int *elist, int *elen, std::set<int> * X, std::set<int> * Y, int * pi,
	bool update_pi, std::vector<int> * deg_not_2, std::vector<int> * tree_edges) {
	int min_w = 1000000, bound, i, ignore, SMALL_LEN = -10000000, BIG_LEN = 10000000;
	int * v = new int[ncount], *min_v = new int[ncount], * elen_new = new int[ecount];
	std::vector<int> edges;
	std::vector<edge> E;

	deg_not_2->erase(deg_not_2->begin(),deg_not_2->end());

	for( i = 0; i < ecount; i++ ) elen_new[i] = elen[i] + pi[elist[2*i]] + pi[elist[2*i+1]];

	// Sort all the edges by the modified weight
	for( i = 0; i < ecount; i++ ) {
		if( X->find(i) != X->end() ) {
			// We want to force this edge into the tree, so give it a small weight
			E.push_back(edge(elist[2*i], elist[2*i+1], SMALL_LEN, i));
		} else if( Y->find(i) != Y->end() ) {
			// We want to force this edge into the tree, so give it a large weight
			E.push_back(edge(elist[2*i], elist[2*i+1], BIG_LEN, i));
		} else {
			// Give this edge the weight modified by pi
			E.push_back(edge(elist[2*i], elist[2*i+1], elen_new[i], i));
		}
	}
	std::sort(E.begin(), E.end());
	
	// Compute a sequence of 1-trees, cycling through which node is ignored in MST computation
	for( ignore = 0; ignore < ncount; ignore++ ) {
		edges.erase(edges.begin(), edges.end());
		bound = w_candidate(ncount, ecount, elen, &E, ignore, pi, v, &edges);
		if( bound <= min_w ) {
			min_w = bound;
			memcpy(min_v, v, ncount*sizeof(int));
			(*tree_edges) = edges;
		}
	}

	// Update pi
	for( i = 0; i < ncount; i++ ) {
		// Find nodes without degree 2
		if( min_v[i] != 0 ) {
			deg_not_2->push_back(i);
		}
		if( update_pi ) {
			pi[i] += min_v[i]*t_bar;
		}
	}

	delete [] v;
	delete [] min_v;
	delete [] elen_new;

	return min_w;
}

int w_candidate(int ncount, int ecount, int * elen, std::vector<edge> * E, int ignore, int * pi, 
		int * v, std::vector<int> * tree_edges) {
	int e1 = -1, e2 = -1, edges_found = 0, i, tot = 0;;
	tnode *nodes = new tnode[ncount];
	std::vector<int> mst;

	for( i = 0; i < ncount; i++ ) nodes[i].init(i);

	mst.reserve(ncount-2);

	// Run kruskal's algorithm, making sure to ignore the desired node
	for( i = 0; i < ecount; i++ ) {
		if( E->at(i).end1 == ignore || E->at(i).end2 == ignore ) {
			if( edges_found == 0 ) {
				e1 = i;
				edges_found++;
			} else if( edges_found == 1 ) {
				e2 = i;
				edges_found++;
			}
		} else if( nodes[E->at(i).end1].find_label() != nodes[E->at(i).end2].find_label() ) {
			mst.push_back(i);
			nodes[E->at(i).end1].join(&(nodes[(E->at(i).end2)]));
		} 
		if( (int)mst.size() == ncount-2 && edges_found == 2 ) {
			break;
		}
	}

	// If for some reason branching cause graph to be disconnected, return an artificially large bound
	if ( (int)mst.size() != ncount-2 || e1 == -1 || e2 == -1 ) {
		delete [] nodes;

		return 100000000;
	}

	// Compute w (lower bound)
	for(  i = 0; i < ncount; i++ ) v[i] = -2;

	for( i = 0; i < (int)mst.size(); i++ ) {
		tot += elen[E->at(mst[i]).ind];
		tree_edges->push_back(E->at(mst[i]).ind);
		v[E->at(mst[i]).end1]++;
		v[E->at(mst[i]).end2]++;
	}

	tot += elen[E->at(e1).ind];
	tot += elen[E->at(e2).ind];

	tree_edges->push_back(E->at(e1).ind);
	tree_edges->push_back(E->at(e2).ind);

	v[E->at(e1).end1]++;
	v[E->at(e1).end2]++;
	v[E->at(e2).end1]++;
	v[E->at(e2).end2]++;

	for( i = 0; i < ncount; i++ ) {
		tot += pi[i] * v[i];
	}

	delete [] nodes;

	return tot;
}
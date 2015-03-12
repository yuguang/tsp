#include "onetree.h"
#include "edge.h"
#include "graph.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <set>
#include <queue>
#include <cassert>

const int t_bar = 1;
const int p = 20;

bool do_print_tour = false;

branch_node::branch_node(std::set<int> X, std::set<int> Y, int ncount, int *pi, int w) {
	this->X = X;
	this->Y = Y;
	this->pi = new int[ncount];
	assert( pi != 0 );
	memcpy(this->pi, pi, ncount*sizeof(int));
	this->w = w;
}

branch_node::~branch_node() {
	std::cout << "going to delete pi" << std::endl;
	if( pi ) {
		delete [] pi; pi = 0;
	}
	std::cout << "deleted pi" << std::endl;
}

bool operator<(const branch_node & l, const branch_node & r) {
	return l.w > r.w; // Because we want a min heap we flip the sign
}

int one_tree_tsp(int ncount, int ecount, int *elist, int *elen, int upper_bound) {
	int branches = 0;
	int *pi = new int[ncount];
	int bound, max_w = -100000, max_w_p_ago = -100000;
	int *pi_prime = new int[ncount];
	bool do_branch;
	std::priority_queue<branch_node> Q;

	for( int i = 0; i < ncount; i++ ) pi[i] = 0;
		

	std::vector<int> w_cache;
	w_cache.reserve(p);

	std::set<int> X, Y;
	std::vector<int> deg_not_2, deg_not_2_prime;

	bound = w(ncount, ecount, elist, elen, X, Y, pi, false, &deg_not_2);

	branch_node start = branch_node(X,Y,ncount,pi,bound);
	Q.push(start);

	while( Q.size() != 0 ) {
		branch_node current = Q.top();		
		bound = current.w;	

		if( bound > upper_bound ) {
			continue;
		}

		memcpy(pi, current.pi, ncount*sizeof(int));
		X = current.X;
		Y = current.Y;

		Q.pop(); branches++;

		std::set<int>::iterator it;
		// std::cout << "Popped (" << current.w << ")" << std::endl;
		// std::cout << "pi: "; for( int i = 0; i < ncount; i++ ) std::cout << pi[i] << " "; std::cout << std::endl;
		// std::cout << "X: "; for( it = X.begin(); it != X.end(); it++ ) std::cout << (*it) << " "; std::cout << std::endl;
		// std::cout << "Y: "; for( it = Y.begin(); it != Y.end(); it++ ) std::cout << (*it) << " "; std::cout << std::endl;

		do_branch = true;
		max_w = -100000;
		max_w_p_ago = -100000;
		deg_not_2.erase(deg_not_2.begin(),deg_not_2.end());
		bound = w(ncount, ecount, elist, elen, X, Y, pi, false, &deg_not_2);
		if( deg_not_2.size() == 0 ) {
			// std::cout << "FOUND TOUR 1" << std::endl;
			std::cout << "branches: " << branches << std::endl;
			delete [] pi; pi = 0;
			delete [] pi_prime; pi_prime = 0;
			return bound;
		}

		for(int i = 0; ; i++) {
			deg_not_2.erase(deg_not_2.begin(),deg_not_2.end());
			bound = w(ncount, ecount, elist, elen, X, Y, pi, true, &deg_not_2);

			if( i >= p ) {
				if( w_cache[i%p] > max_w_p_ago ) {
					max_w_p_ago = w_cache[i%p];
				}
			}
			w_cache[i%p] = bound;
			if( bound > max_w ) {
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
		if( deg_not_2_prime.size() == 0 ) {
			// std::cout << "FOUND TOUR 2" << std::endl;
			// std::cout << "branches: " << branches << std::endl;
			bound = w(ncount, ecount, elist, elen, X, Y, pi_prime, false, &deg_not_2);
			Q.push(branch_node(X,Y,ncount,pi_prime,bound));
		}
		if( do_branch ) {
			bool done = false;
			for( unsigned i = 0; i < deg_not_2_prime.size(); i++ ) {
				for( int j = 0; j < ecount; j++ ) {
					if( (elist[2*j] == deg_not_2_prime[i] || elist[2*j+1] == deg_not_2_prime[i]) && X.find(j) == X.end() && Y.find(j) == Y.end() ) {
						std::set<int> X_new = X;
						std::set<int> Y_new = Y;
						X_new.insert(j);
						Y_new.insert(j);
						bound = w(ncount, ecount, elist, elen, X_new, Y, pi_prime, false, &deg_not_2);
						Q.push(branch_node(X_new,Y,ncount,pi_prime,bound));
						bound = w(ncount, ecount, elist, elen, X, Y_new, pi_prime, false, &deg_not_2);
						Q.push(branch_node(X,Y_new,ncount,pi_prime,bound));
						done = true;
						break;
					}
				}
				if( done ) {
					break;
				}
			}
		}

	}

	std::cout << "branches: " << branches << std::endl;
	delete [] pi; pi = 0;
	delete [] pi_prime; pi_prime = 0;
	return upper_bound;
}

int w(int ncount, int ecount, int *elist, int *elen, std::set<int> X, std::set<int> Y, int * pi,  bool update_pi, std::vector<int> * deg_not_2) {
	int min_w = 1000000;
	int * v = new int[ncount], *min_v = new int[ncount];
	int * elen_new = new int[ecount];
	std::vector<edge> edges;

	for( int i = 0; i < ecount; i++ ) elen_new[i] = elen[i] + pi[elist[2*i]] + pi[elist[2*i+1]];

	int bound;
	for( int ignore = 0; ignore < ncount; ignore++ ) {
		bound = w_candidate(ncount, ecount, elist, elen_new, ignore, pi, v, X, Y); //changes v
		if( bound < min_w ) {
			min_w = bound;
			memcpy(min_v, v, ncount*sizeof(int));
		}
	}

	// Update pi
	for( int i = 0; i < ncount; i++ ) {
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

int w_candidate(int ncount, int ecount, int *elist, int *elen, int ignore, int * pi, int * v, std::set<int> X, std::set<int> Y) {
	std::vector<edge> edges;
	int SMALL_LEN = -1000000;

	std::set<int>::iterator it;

	// sort only edges incident to node ignoring
	for( int i = 0; i < ecount; i++ ) {
		if(elist[2*i] == ignore || elist[2*i+1] == ignore) {
			if( X.find(i) != X.end() ) {
				edges.push_back(edge(elist[2*i], elist[2*i+1], SMALL_LEN, i));
			} else {
				edges.push_back(edge(elist[2*i], elist[2*i+1], elen[i], i));
			}
		}
	}
	std::sort(edges.begin(), edges.end());

	int ecount_sub = ecount - edges.size();

	int ncount_sub = ncount - 1;
	int * elist_sub = new int[ecount_sub*2];
	int * elen_sub = new int[ecount_sub];
	std::vector<int> must_include;
	int * orig_ind = new int[ecount];

	int j = 0;
	for( int i = 0; i < ecount; i++ ) {
		if(elist[2*i] != ignore  && elist[2*i+1] != ignore ) {
			if( Y.find(i) != Y.end() ) {
				ecount_sub--;
				continue;
			}
			if( elist[2*i] > ignore ) {
				elist_sub[2*j] = elist[2*i]-1;	
			} else {
				elist_sub[2*j] = elist[2*i];
			}
			if( elist[2*i+1] > ignore ) {
				elist_sub[2*j+1] = elist[2*i+1]-1;
			} else {
				elist_sub[2*j+1] = elist[2*i+1];
			}
			if( X.find(i) != X.end() ) {
				must_include.push_back(j);
			}

			elen_sub[j] = elen[i];
			orig_ind[j] = i;
			j++;
		}
	}

	graph G;
	G.init(ncount_sub, ecount_sub, elist_sub, elen_sub);

	std::vector<int> mst = G.min_spanning_tree(must_include);

	int tot = 0;
	for( unsigned i = 0; i < mst.size(); i++ ) {
		tot += elen_sub[mst[i]];
	}
	if ( mst.size() != ncount_sub - 1 ) {
		return 1000;
	}

	for( int i = 0; i < ncount; i++ ) v[i] = 0;

	tot += elen[edges[0].ind];
	tot += elen[edges[1].ind];

	if( edges[0].end1 == ignore ) {
		v[edges[0].end2] += 1;
	} else {
		v[edges[0].end1] += 1;
	}
	if( edges[1].end1 == ignore ) {
		v[edges[1].end2] += 1;
	} else {
		v[edges[1].end1] += 1;
	}
	int end1, end2;
	v[ignore] = 2;



	for( int i = 0; i < ncount_sub-1; i++ ) {
		end1 = elist_sub[2*mst[i]];
		end2 = elist_sub[2*mst[i]+1];
		if( end1 >= ignore ) end1++;
		if( end2 >= ignore ) end2++;
		v[end1]++;
		v[end2]++;
	}

	for( int i = 0; i < ncount; i++ ) v[i] -= 2;

	for( int i = 0; i < ncount; i++ ) {
		tot += pi[i] * v[i];
	}

	if( do_print_tour ) {
		std::cout << edges[0].ind << " " << edges[1].ind;
		for( unsigned i = 0; i < mst.size(); i++ ) {
			std::cout << " " << orig_ind[mst[i]];
		}
		for( int i = 0; i < ncount; i++ ) std::cout << v[i] << " ";
		std::cout << std::endl;
	}

	delete [] elist_sub;
	delete [] elen_sub;
	delete [] orig_ind;

	return tot;
}
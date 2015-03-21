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
const int p = 25;

bool do_print_tour = false;
branch_node::branch_node(std::set<int> X, std::set<int> Y, int ncount, int *pi, int w) {
	this->X = X;
	this->Y = Y;
	this->pi = new int[ncount];
	assert( pi != 0 );
	memcpy(this->pi, pi, ncount*sizeof(int));
	this->w = w;
	this->ncount = ncount;
}

branch_node::branch_node(const branch_node& other) {
	this->X = other.X;
	this->Y = other.Y;
	this->pi = new int[other.ncount];
	assert( other.pi != 0 );
	memcpy(this->pi, other.pi, other.ncount*sizeof(int));
	this->w = other.w;
	this->ncount = other.ncount;
}

branch_node & branch_node::operator=(const branch_node& other) {
		this->X = other.X;
		this->Y = other.Y;
		assert( other.pi != 0 );
		memcpy(this->pi, other.pi, other.ncount*sizeof(int));
		this->w = other.w;
		this->ncount = other.ncount;

		return (*this);
}

branch_node::~branch_node() {
	if( pi ) {
		delete [] pi; pi = 0; // Not sure why this causes crashes !?
	}
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
	std::vector<int> edges;

	for( int i = 0; i < ncount; i++ ) pi[i] = 0;
		

	// std::cout << "elist" << std::endl;
	// for( int i = 0; i < ecount; i++ ) {
	// 	std::cout << i << " " << elist[2*i] << " " << elist[2*i+1] << std::endl;
	// }

	std::vector<int> w_cache;
	w_cache.reserve(p);

	std::set<int> X, Y;
	std::vector<int> deg_not_2, deg_not_2_prime;

	bound = w(ncount, ecount, elist, elen, X, Y, pi, false, &deg_not_2, &edges);

	branch_node start = branch_node(X,Y,ncount,pi,bound);
	Q.push(start);

	while( Q.size() != 0 ) {
		branch_node current = Q.top();
		bound = current.w;	
		// std::cout << bound << std::endl;

		if( bound > upper_bound ) {
			// std::cout << "bad upper bound" << std::endl;
			// std::cout << "pi: "; for( int i = 0; i < ncount; i++ ) std::cout << current.pi[i] << " "; std::cout << std::endl;
			Q.pop(); branches++;
			continue;
		}

		memcpy(pi, current.pi, ncount*sizeof(int));
		X = current.X;
		Y = current.Y;

		Q.pop(); branches++;

		std::set<int>::iterator it;
		// std::cout << "Popped (" << current.w << "), branches: " << branches << ", Q size: " << Q.size() << std::endl;
		// std::cout << "pi: "; for( int i = 0; i < ncount; i++ ) std::cout << pi[i] << " "; std::cout << std::endl;
		// std::cout << "X: "; for( it = X.begin(); it != X.end(); it++ ) std::cout << (*it) << " "; std::cout << std::endl;
		// std::cout << "Y: "; for( it = Y.begin(); it != Y.end(); it++ ) std::cout << (*it) << " "; std::cout << std::endl;

		do_branch = true;
		max_w = -100000;
		max_w_p_ago = -100000;
		deg_not_2.erase(deg_not_2.begin(),deg_not_2.end());
		bound = w(ncount, ecount, elist, elen, X, Y, pi, false, &deg_not_2, &edges);
		if( deg_not_2.size() == 0 ) {
			// std::cout << "trying to end" << std::endl;
			// for( int j = 0; j < ncount; j++ ) {
			// 	std::cout << edges[j] << std::endl;
			// }
			int last = elist[2*edges[0]];
			std::cout << "One Tree Tour: " << last;
			edges.erase(edges.begin(),edges.begin()+1);
			for( int j = 0; j < ncount-1; j++ ) {
				unsigned i;
				for( i = 0; i < edges.size(); i++ ) {
					int edge = edges[i];
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

		for(int i = 0; ; i++) {
			deg_not_2.erase(deg_not_2.begin(),deg_not_2.end());
			// std::cout << "started w" << std::endl;
			bound = w(ncount, ecount, elist, elen, X, Y, pi, true, &deg_not_2, &edges);
			// std::cout << "back from w" << std::endl;
			// std::cout << "pi: "; for( int j = 0; j < ncount; j++ ) std::cout << pi[j] << " "; std::cout << std::endl;

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
			if( bound > upper_bound ) {
				do_branch = false;
				break;
			}
			if( max_w_p_ago == max_w ) {
				break;
			}
		}
		// std::cout << "iterated pi, branching: " << do_branch << std::endl;
		if( deg_not_2_prime.size() == 0 ) {
			// std::cout << "FOUND TOUR 2" << std::endl;
			// std::cout << "branches: " << branches << std::endl;
			bound = w(ncount, ecount, elist, elen, X, Y, pi_prime, false, &deg_not_2, &edges);
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
						bound = w(ncount, ecount, elist, elen, X_new, Y, pi_prime, false, &deg_not_2, &edges);
						Q.push(branch_node(X_new,Y,ncount,pi_prime,bound));
						bound = w(ncount, ecount, elist, elen, X, Y_new, pi_prime, false, &deg_not_2, &edges);
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

	delete [] pi; pi = 0;
	delete [] pi_prime; pi_prime = 0;
	return upper_bound;
}

int w(int ncount, int ecount, int *elist, int *elen, std::set<int> X, std::set<int> Y, int * pi,
	bool update_pi, std::vector<int> * deg_not_2, std::vector<int> * tree_edges) {
	int min_w = 1000000, bound, i, ignore;
	int * v = new int[ncount], *min_v = new int[ncount], * elen_new = new int[ecount];
	int SMALL_LEN = -10000000, BIG_LEN = 10000000;
	std::vector<int> edges;

	for( i = 0; i < ecount; i++ ) elen_new[i] = elen[i] + pi[elist[2*i]] + pi[elist[2*i+1]];

	std::vector<edge> E;
	for( int i = 0; i < ecount; i++ ) {
		if( X.find(i) != X.end() ) {
			E.push_back(edge(elist[2*i], elist[2*i+1], SMALL_LEN, i));
		} else if( Y.find(i) != Y.end() ) {
			E.push_back(edge(elist[2*i], elist[2*i+1], BIG_LEN, i));
		} else {
			E.push_back(edge(elist[2*i], elist[2*i+1], elen_new[i], i));
		}
	}
	std::sort(E.begin(), E.end());
	
	for( ignore = 0; ignore < ncount; ignore++ ) {
		edges.erase(edges.begin(), edges.end());
		// bound = w_candidate(ncount, ecount, elist, elen, elen_new, ignore, pi, v, X, Y, &edges);
		bound = w_candidate(ncount, ecount, elen, &E, ignore, pi, v, &edges);
		// std::cout << "bound: " << bound << std::endl;
		if( bound <= min_w ) {
			min_w = bound;
			memcpy(min_v, v, ncount*sizeof(int));
			(*tree_edges) = edges;
		}
	}

	// std::cout << "back from bounds" << std::endl;

	// Update pi
	for( i = 0; i < ncount; i++ ) {
		if( min_v[i] != 0 ) {
			deg_not_2->push_back(i);
		}
		if( update_pi ) {
			pi[i] += min_v[i]*t_bar;
		}
	}

	// std::cout << "updated pi" << std::endl;
	// std::cout << "pi: "; for( int i = 0; i < ncount; i++ ) std::cout << pi[i] << " "; std::cout << std::endl;


	delete [] v;
	delete [] min_v;
	delete [] elen_new;

	return min_w;
}

// int w_candidate(int ncount, int ecount, int *elist, int * elen, int *elen_new, int ignore, int * pi, 
		// int * v, std::set<int> X, std::set<int> Y, std::vector<int> * tree_edges) {
int w_candidate(int ncount, int ecount, int * elen, std::vector<edge> * E, int ignore, int * pi, 
		int * v, std::vector<int> * tree_edges) {

	tnode *nodes = new tnode[ncount];
	std::vector<int> mst;

	for( int i = 0; i < ncount; i++ ) nodes[i].init(i);

	mst.reserve(ncount-2);
	int e1 = -1, e2 = -1;
	int edges_found = 0;

	// std::cout << "Looking for mst" << std::endl;

	for( int i = 0; i < ecount; i++ ) {
		if( E->at(i).end1 == ignore || E->at(i).end2 == ignore ) {
			if( edges_found == 0 ) {
				e1 = i;
				// std::cout << "Found e1" << std::endl;
				edges_found++;
			} else if( edges_found == 1 ) {
				e2 = i;
				// std::cout << "Found e2" << std::endl;
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

	if ( (int)mst.size() != ncount-2 || e1 == -1 || e2 == -1 ) {
		delete [] nodes;

		return 100000000;
	}

	// std::cout << "Found mst" << std::endl;
	

	// std::vector<edge> edges;
	// int SMALL_LEN = -1000000;

	// std::set<int>::iterator it;

	// // sort only edges incident to node ignoring
	// for( int i = 0; i < ecount; i++ ) {
	// 	if(elist[2*i] == ignore || elist[2*i+1] == ignore) {
	// 		if( X.find(i) != X.end() ) {
	// 			edges.push_back(edge(elist[2*i], elist[2*i+1], SMALL_LEN, i));
	// 		} else {
	// 			edges.push_back(edge(elist[2*i], elist[2*i+1], elen_new[i], i));
	// 		}
	// 	}
	// }
	// std::sort(edges.begin(), edges.end());

	// int ecount_sub = ecount - edges.size();

	// int ncount_sub = ncount - 1;
	// int * elist_sub = new int[ecount_sub*2];
	// int * elen_sub = new int[ecount_sub];
	// std::vector<int> must_include;
	// int * orig_ind = new int[ecount];

	// int j = 0;
	// for( int i = 0; i < ecount; i++ ) {
	// 	if(elist[2*i] != ignore  && elist[2*i+1] != ignore ) {
	// 		if( Y.find(i) != Y.end() ) {
	// 			ecount_sub--;
	// 			continue;
	// 		}
	// 		if( elist[2*i] > ignore ) {
	// 			elist_sub[2*j] = elist[2*i]-1;	
	// 		} else {
	// 			elist_sub[2*j] = elist[2*i];
	// 		}
	// 		if( elist[2*i+1] > ignore ) {
	// 			elist_sub[2*j+1] = elist[2*i+1]-1;
	// 		} else {
	// 			elist_sub[2*j+1] = elist[2*i+1];
	// 		}
	// 		if( X.find(i) != X.end() ) {
	// 			must_include.push_back(j);
	// 		}

	// 		elen_sub[j] = elen_new[i];
	// 		orig_ind[j] = i;
	// 		j++;
	// 	}
	// }

	// graph G;
	// G.init(ncount_sub, ecount_sub, elist_sub, elen_sub);

	// std::vector<int> mst = G.min_spanning_tree(must_include);

	// std::cout << "Updating tots" << std::endl;

	int tot = 0;
	for( int i = 0; i < ncount; i++ ) v[i] = -2;
	for( unsigned i = 0; i < mst.size(); i++ ) {
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

	// tree_edges->push_back(E->at(e1).ind);
	// tree_edges->push_back(E->at(e1).ind);

	// if( E[e1].end1 == ignore ) {
	// 	v[E[e1].end2] += 1;
	// } else {
	// 	v[E[e1].end1] += 1;
	// }
	// if( E[e2].end1 == ignore ) {
	// 	v[E[e2].end2] += 1;
	// } else {
	// 	v[E[e2].end1] += 1;
	// }
	// v[ignore] = 0;

	// for( int i = 0; i < ncount-2; i++ ) {
	// 	end1 = E->at(mst[i]).end1;
	// 	end2 = E->at(tree_edges->at(i)).end2;
	// 	v[E->at(mst[i]).end1]++;
	// 	v[E->at(mst[i]).end2]++;
	// }

	// for( int i = 0; i < ncount_sub-1; i++ ) {
	// 	end1 = elist[2*orig_ind[mst[i]]];
	// 	end2 = elist[2*orig_ind[mst[i]]+1];
	// 	v[end1]++;
	// 	v[end2]++;
	// }

	for( int i = 0; i < ncount; i++ ) {
		tot += pi[i] * v[i];
	}

	// std::cout << "Done updating tots" << std::endl;

	delete [] nodes;

	return tot;
}
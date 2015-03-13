#include "bhk2.h"
#include <limits>
#include <cstdio>

bhk2::bhk2() {

}

bhk2::~bhk2() {
	
}

void bhk2::init(int ncount, int **dmatrix) {
	this->ncount = ncount;
	this->dmatrix = dmatrix;
	this->opt = new int*[ncount - 1];
	for(int i = 0; i < (ncount-1); ++i) {
		opt[i] = new int[1<<(ncount-1)];
	}
}

int bhk2::solve(Set S, int t) {
	int i, v, minv = std::numeric_limits<int>::max();
	Set R;
	if (opt[t][S] == -1) {
		R = S & ~(1<<t);
		for (i=0; i<this->ncount-1; i++) {
			if (!(R & (1<<i))) continue;
			v = this->solve(R,i) + this->dmatrix[i][t];
			if (v < minv) minv = v;
		}
		opt[t][S] = minv;
	}
	return opt[t][S];
}

int bhk2::solve() {

	Set S, setcount = 1<<(this->ncount-1);
	for (int i=0; i<this->ncount-1; i++) {
		for (S=0; S<setcount; S++) {
			this->opt[i][S] = -1;
		}
		this->opt[i][1<<i] = this->dmatrix[i][this->ncount-1];
	}
	
	int t, len, bestlen=std::numeric_limits<int>::max();
	Set N=(1<<(this->ncount-1))-1;
	for (t=0; t<this->ncount-1; t++) {
		len = this->solve(N,t) + this->dmatrix[t][this->ncount-1];
		if (len < bestlen) bestlen = len;
	}
	
	return bestlen;
}


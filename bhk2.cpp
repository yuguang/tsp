#include "bhk2.h"
#include <limits>
#include <cstdio>

bhk2::bhk2() {

}

bhk2::~bhk2() {
	
}

int bhk2::getDistance(int city1, int city2) {
	if (city2 < city1) {
		return this->getDistance(city2, city1);
	}
	std::pair<int,int> key = std::make_pair(city1,city2);
	if (this->distance.count(key) > 0) {
		return this->distance[std::make_pair(city1,city2)];
	}
	return std::numeric_limits<int>::max(); // if there is no edge, we return INT_MAX
}

void bhk2::init(int ncount, int ecount, int *elist, int *elen) {
	this->ncount = ncount;
	this->ecount = ecount;
	this->elist = elist;
	this->elen = elen;
	for (int i = 0; i < ecount; i++) {
		std::pair<int,int> key;
		// we want the lower numbered city to be first:
		if (elist[2*i] < elist[2*i+1]) {
			key = std::make_pair(elist[2*i],elist[2*i+1]);
		} else {
			key = std::make_pair(elist[2*i+1],elist[2*i]);
		}
		this->distance[key] = elen[i];
	}
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
			v = this->solve(R,i) + this->getDistance(i,t);
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
		this->opt[i][1<<i] = this->getDistance(i,ncount-1);
	}
	
	int t, len, bestlen=std::numeric_limits<int>::max();
	Set N=(1<<(this->ncount-1))-1;
	for (t=0; t<this->ncount-1; t++) {
		len = this->solve(N,t) + this->getDistance(t,this->ncount-1);
		if (len < bestlen) bestlen = len;
	}
	
	return bestlen;
}


#include "bhk.h"
#include <vector>
#include <limits>

bhk::bhk() {

}

bhk::~bhk() {
	
}

int bhk::getDistance(int city1, int city2) {
	std::pair<int,int> key = std::make_pair(city1,city2);
	if (this->distance.count(key) > 0) {
		return this->distance[std::make_pair(city1,city2)];
	}
	return std::numeric_limits<int>::max(); // if there is no edge, we return INT_MAX
}

void bhk::init(int ncount, int ecount, int *elist, int *elen) {
	this->ncount = ncount;
	this->ecount = ecount;
	this->elist = elist;
	this->elen = elen;
	for (int i = 0; i < ecount; i++) {
		this->distance[std::make_pair(elist[2*i],elist[2*i+1])];
	}
}

double bhk::solve() {
	std::map<std::vector<int>,int> DPmap; // stores dynamic programming results
	for (int i = 1; i < this->ncount; i++) {
		std::vector<int> mapKey;
		mapKey.push_back(0);
		mapKey.push_back(i);
		DPmap[mapKey] = this->getDistance(0,i);
	}
	// TODO: rest of the algorithm
	return 0.0;
}


#include "bhk.h"
#include <limits>

bhk::bhk() {

}

bhk::~bhk() {
	
}

int bhk::getDistance(int city1, int city2) {
	if (city2 < city1) {
		return this->getDistance(city2, city1);
	}
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

std::vector<std::vector<int> > bhk::getSubsets(int size, int start, int end) {
	std::vector<std::vector<int> > rval;
	if (size == 0) {
		return rval;
	}
	if (size == (end - start + 1)) {
		std::vector<int> subset;
		for (int i = start; i <= end; i++) {
			subset.push_back(i);
		}
		rval.push_back(subset);
		return rval;
	}
	std::vector<std::vector<int> > with_end = this->getSubsets(size - 1, start, end - 1);
	std::vector<std::vector<int> > without_end = this->getSubsets(size, start, end - 1);
	for (unsigned int i = 0; i < with_end.size(); i++) {
		with_end[i].push_back(end);
	}
	// copied from http://stackoverflow.com/questions/3177241/best-way-to-concatenate-two-vectors
	rval.reserve( with_end.size() + without_end.size() ); // preallocate memory
	rval.insert( rval.end(), with_end.begin(), with_end.end() );
	rval.insert( rval.end(), without_end.begin(), without_end.end() );
	return rval;
}

int bhk::solve() {
	std::map<std::pair<std::vector<int>,int>,int> DPmap; // stores dynamic programming results
	for (int i = 1; i < this->ncount; i++) {
		std::vector<int> keyVector;
		keyVector.push_back(0);
		keyVector.push_back(i);
		DPmap[std::make_pair(keyVector,i)] = this->getDistance(0,i);
	}
	for (int subset_size = 3; subset_size <= this->ncount; subset_size++) {
		std::vector<std::vector<int> > subsets = this->getSubsets(subset_size, 0, this->ncount - 1);
		for (unsigned int i = 0; i < subsets.size(); i++) {
			for (int j = 0; j < subset_size; j++) {
				int endCity = subsets[i][j];
				int minVal = std::numeric_limits<int>::max();
				for (int k = 1; k < subset_size; k++) {
					if (k != j) {
						std::vector<int> test_subset = subsets[i];
						test_subset.erase(test_subset.begin() + k - 1);
						int test_val = DPmap[std::make_pair(test_subset,endCity)] + this->getDistance(endCity,subsets[i][k]);
						if (test_val < minVal) {
							minVal = test_val;
						}
					}
				}
				DPmap[std::make_pair(subsets[i],endCity)] = minVal;
			}
		}
	}
	std::vector<int> allCities;
	for (int i = 0; i < this->ncount; i++) {
		allCities.push_back(i);
	}
	int rVal = std::numeric_limits<int>::max();
	for (int i = 1; i < this->ncount; i++) {
		int testVal = DPmap[std::make_pair(allCities,i)] + this->getDistance(0,i);
		if (testVal < rVal) {
			rVal = testVal;
		}
	}
	return rVal;
}


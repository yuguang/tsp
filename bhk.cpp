#include "bhk.h"
#include <limits>
#include <cstdio>

bhk::bhk() {

}

bhk::~bhk() {
	
}

void bhk::init(int ncount, int **dmatrix) {
	this->ncount = ncount;
	this->dmatrix = dmatrix;
}

std::vector<unsigned long long> bhk::getSubsets(int size, int end) {
	// returns all subsets of {1, ..., end} with size elements
	std::vector<unsigned long long> rval;
	if (size == 1) {
		for (int i = 1; i <= end; i++) {
			rval.push_back(1<<((unsigned long long) i-1));
		}
		return rval;
	}
	if (size == end) {
		// in this case we know all the cities must be included
		unsigned long long subset = 0;
		for (int i = 1; i <= end; i++) {
			subset += 1<<((unsigned long long) i-1);
		}
		rval.push_back(subset);
		return rval;
	}
	std::vector<unsigned long long> withEnd = this->getSubsets(size - 1, end - 1);
	std::vector<unsigned long long> withoutEnd = this->getSubsets(size, end - 1);
	for (unsigned int i = 0; i < withEnd.size(); i++) {
		withEnd[i] += (1<<((unsigned long long) end-1));
	}
	// copied from http://stackoverflow.com/questions/3177241/best-way-to-concatenate-two-vectors
	rval.reserve( withEnd.size() + withoutEnd.size() ); // preallocate memory
	rval.insert( rval.end(), withEnd.begin(), withEnd.end() );
	rval.insert( rval.end(), withoutEnd.begin(), withoutEnd.end() );
	return rval;
}

int bhk::solve() {
	std::map<std::pair<unsigned long long,int>,int> DPmap; // stores dynamic programming results
	// all subsets should include city 0, so to save space we don't bother adding it in
	for (int i = 1; i < this->ncount; i++) {
	    unsigned long long subset = 1<<((unsigned long long) i-1);
		DPmap[std::make_pair(subset,i)] = this->dmatrix[0][i];
		//this->printDPval(subset,i,this->dmatrix[0][i]);
	}
	for (int subset_size = 2; subset_size < this->ncount; subset_size++) {
		std::vector<unsigned long long> subsets = this->getSubsets(subset_size, this->ncount - 1);
		for (unsigned int i = 0; i < subsets.size(); i++) {
			for (int j = 1; j < this->ncount; j++) {
				// j is the city we want to add to the path
				if (subsets[i] & (1<<((unsigned long long) j-1))) {
					unsigned long long subset_minus_j = subsets[i] - (1<<((unsigned long long) j-1));
					int minVal = std::numeric_limits<int>::max();
					for (int k = 1; k < this->ncount; k++) {
						// loop through possible second last cities
						if (subset_minus_j & (1<<((unsigned long long) k-1))) { // if k is acutally in subset_minus_j
							int testVal = DPmap[std::make_pair(subset_minus_j,k)] + this->dmatrix[k][j];
							if (testVal < minVal) {
								minVal = testVal;
							}
						}
					}
					DPmap[std::make_pair(subsets[i],j)] = minVal;
					//this->printDPval(subsets[i],j,minVal);
				}
			}
		}
	}
	unsigned long long allCities = 0;
	for (int i = 1; i < this->ncount; i++) {
		allCities += 1<<((unsigned long long) i-1);
	}
	int rVal = std::numeric_limits<int>::max();
	for (int i = 1; i < this->ncount; i++) {
		int testVal = DPmap[std::make_pair(allCities,i)] + this->dmatrix[0][i];
		if (testVal < rVal) {
			rVal = testVal;
		}
	}
	return rVal;
}

void bhk::printDPval(unsigned long long subset, int endcity, int value) {
	printf("DP[{0");
	for (int i = 1; i < this->ncount; i++) {
		if (subset & 1<<((unsigned long long) i-1))
		printf(",%d", i);
	}
	printf("},%d] = %d\n", endcity, value);
}


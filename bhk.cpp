#include "bhk.h"
#include <limits>
#include <cstdio>

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
		std::pair<int,int> key;
		// we want the lower numbered city to be first:
		if (elist[2*i] < elist[2*i+1]) {
			key = std::make_pair(elist[2*i],elist[2*i+1]);
		} else {
			key = std::make_pair(elist[2*i+1],elist[2*i]);
		}
		this->distance[key] = elen[i];
	}
}

std::vector<std::vector<int> > bhk::getSubsets(int size, int end) {
	// returns all subsets of {1, ..., end} with size elements
	std::vector<std::vector<int> > rval;
	if (size == 1) {
		for (int i = 1; i <= end; i++) {
			std::vector<int> v;
			v.push_back(i);
			rval.push_back(v);
		}
		return rval;
	}
	if (size == end) {
		// in this case we know all the cities must be included
		std::vector<int> subset;
		subset.reserve(size);
		for (int i = 1; i <= end; i++) {
			subset.push_back(i);
		}
		rval.push_back(subset);
		return rval;
	}
	std::vector<std::vector<int> > withEnd = this->getSubsets(size - 1, end - 1);
	std::vector<std::vector<int> > withoutEnd = this->getSubsets(size, end - 1);
	for (unsigned int i = 0; i < withEnd.size(); i++) {
		withEnd[i].push_back(end);
	}
	// copied from http://stackoverflow.com/questions/3177241/best-way-to-concatenate-two-vectors
	rval.reserve( withEnd.size() + withoutEnd.size() ); // preallocate memory
	rval.insert( rval.end(), withEnd.begin(), withEnd.end() );
	rval.insert( rval.end(), withoutEnd.begin(), withoutEnd.end() );
	return rval;
}

int bhk::solve() {
	std::map<std::pair<std::vector<int>,int>,int> DPmap; // stores dynamic programming results
	// all subsets should include city 0, so to save space we don't bother adding it in
	for (int i = 1; i < this->ncount; i++) {
		std::vector<int> keyVector;
		keyVector.push_back(i);
		DPmap[std::make_pair(keyVector,i)] = this->getDistance(0,i);
		//this->printDPval(keyVector,i,this->getDistance(0,i));
	}
	for (int subset_size = 2; subset_size < this->ncount; subset_size++) {
		std::vector<std::vector<int> > subsets = this->getSubsets(subset_size, this->ncount - 1);
		for (unsigned int i = 0; i < subsets.size(); i++) {
			for (int j = 0; j < subset_size; j++) {
				// subsets[i][j] is the city we want to add to the path
				std::vector<int> subset_minus_j = subsets[i];
				subset_minus_j.erase(subset_minus_j.begin() + j);
				int minVal = std::numeric_limits<int>::max();
				for (int k = 0; k < subset_size; k++) {
					// loop through possible second last cities
					if (k != j) {
						int testVal = DPmap[std::make_pair(subset_minus_j,subsets[i][k])] + this->getDistance(subsets[i][j],subsets[i][k]);
						if (testVal < minVal) {
							minVal = testVal;
						}
					}
				}
				DPmap[std::make_pair(subsets[i],subsets[i][j])] = minVal;
				//this->printDPval(subsets[i],subsets[i][j],minVal);
			}
		}
	}
	std::vector<int> allCities;
	for (int i = 1; i < this->ncount; i++) {
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

void bhk::printDPval(std::vector<int> subset, int endcity, int value) {
	printf("DP[{%d", subset[0]);
	for (unsigned int i = 1; i < subset.size(); i++) {
		printf(",%d", subset[i]);
	}
	printf("},%d] = %d\n", endcity, value);
}


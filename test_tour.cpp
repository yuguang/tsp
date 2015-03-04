#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "bhk.h"

void checkValue(int testNumber, int expectedValue, int testValue) {
	printf("Test %d: ", testNumber);
	if (testValue == expectedValue) {
		printf("PASSED\n");
	} else {
		printf("FAILED\n");
		printf("Expected value: %d. Test value: %d\n", expectedValue, testValue);
	}
}

int main (int ac, char **av) {
	printf("Running tests...\n");

	// Test 1: graph with 2 nodes
	bhk* bhk_solver1 = new bhk();
	int ncount1 = 2;
	int ecount1 = 1;
	int elist1[] = {0,1};
	int elen1[] = {3};
	bhk_solver1->init(ncount1, ecount1, elist1, elen1);
	int result1 = bhk_solver1->solve();
	checkValue(1, 6, result1);
	
	// Test 2: graph with 3 nodes
	bhk* bhk_solver2 = new bhk();
	int ncount2 = 3;
	int ecount2 = 3;
	int elist2[] = {0,1, 0,2, 1,2};
	int elen2[] = {3, 4, 5};
	bhk_solver2->init(ncount2, ecount2, elist2, elen2);
	int result2 = bhk_solver2->solve();
	checkValue(2, 12, result2);

	// Test 3: graph with 4 nodes
	bhk* bhk_solver3 = new bhk();
	int ncount3 = 4;
	int ecount3 = 6;
	int elist3[] = {0,1, 1,2, 2,3, 0,3, 0,2, 1,3};
	int elen3[] = {1,1,1,1,2,2};
	bhk_solver3->init(ncount3, ecount3, elist3, elen3);
	int result3 = bhk_solver3->solve();
	checkValue(3, 4, result3);

	printf("Done.\n");
	return 0;
}

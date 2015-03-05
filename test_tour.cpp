#include <cstdio>
#include <vector>
#include "bhk.h"

void checkValue(const char *method, int expectedValue, int testValue) {
	printf("  %s: ", method);
	if (testValue == expectedValue) {
		printf("\x1b[32mPASSED\x1b[0m\n");
	} else {
		printf("\x1b[31mFAILED\x1b[0m\n");
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
	printf("Test 1:\n");
	checkValue("BHK", 6, bhk_solver1->solve());
	
	// Test 2: graph with 3 nodes
	bhk* bhk_solver2 = new bhk();
	int ncount2 = 3;
	int ecount2 = 3;
	int elist2[] = {0,1, 0,2, 1,2};
	int elen2[] = {3, 4, 5};
	bhk_solver2->init(ncount2, ecount2, elist2, elen2);
	printf("Test 2:\n");
	checkValue("BHK", 12, bhk_solver2->solve());

	// Test 3: graph with 4 nodes
	bhk* bhk_solver3 = new bhk();
	int ncount3 = 4;
	int ecount3 = 6;
	int elist3[] = {0,1, 1,2, 2,3, 0,3, 0,2, 1,3};
	int elen3[] = {1,1,1,1,2,2};
	bhk_solver3->init(ncount3, ecount3, elist3, elen3);
	printf("Test 3:\n");
	checkValue("BHK", 4, bhk_solver3->solve());
	
	// Test 4: graph with 6 nodes
	bhk* bhk_solver4 = new bhk();
	int ncount4 = 6;
	int ecount4 = 15;
	int elist4[] = {0,2, 2,5, 5,1, 1,3, 3,4, 4,0, 0,1, 0,3, 0,5, 1,2, 1,4, 2,3, 2,4, 3,5, 4,5};
	int elen4[] = {2,3,5,4,7,6, 9,8,9,8,9,8,9,8,9};
	bhk_solver4->init(ncount4, ecount4, elist4, elen4);
	printf("Test 4:\n");
	checkValue("BHK", 27, bhk_solver4->solve());

	printf("Done.\n");
	return 0;
}

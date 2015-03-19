#include <cstdio>
#include <vector>
#include <limits>
#include "bhk.h"
#include "bhk2.h"
#include "bhk3.h"

void checkValue(const char *method, int expectedValue, int testValue) {
	printf("  %s: ", method);
	if (testValue == expectedValue) {
		printf("\x1b[32mPASSED\x1b[0m\n");
	} else {
		printf("\x1b[31mFAILED\x1b[0m\n");
		printf("Expected value: %d. Test value: %d\n", expectedValue, testValue);
	}
}

int** distmatrix_from_elist(int ncount, int ecount, int *elist, int *elen) {
    int **distmatrix = new int* [ncount];
    int i,j, end1, end2;
    int default_val = std::numeric_limits<int>::max(); // used for edges that are not specified in elist
    for (i = 0; i < ncount; i++) {
        distmatrix[i] = new int[ncount];
        for (j = 0; j < ncount; j++) {
            distmatrix[i][j] = default_val;
        }
    }
    for (i = 0; i < ecount; i++) {
	    end1 = elist[2*i];
	    end2 = elist[2*i+1];
        distmatrix[end1][end2] = elen[i];
        distmatrix[end2][end1] = elen[i];
    }
    return distmatrix;
}

int main (int ac, char **av) {
	printf("Running tests...\n");

	// Test 1: graph with 2 nodes
	bhk* bhk_solver1 = new bhk();
	bhk2* bhk2_solver1 = new bhk2();
	bhk3* bhk3_solver1 = new bhk3();
	int ncount1 = 2;
	int ecount1 = 1;
	int elist1[] = {0,1};
	int elen1[] = {3};
	int **distmatrix1 = distmatrix_from_elist(ncount1, ecount1, elist1, elen1);
	bhk_solver1->init(ncount1, distmatrix1);
	bhk2_solver1->init(ncount1, distmatrix1);
	bhk3_solver1->init(ncount1, distmatrix1);
	printf("Test 1:\n");
	checkValue("BHK", 6, bhk_solver1->solve());
	checkValue("BHK2", 6, bhk2_solver1->solve());
	checkValue("BHK3", 6, bhk3_solver1->solve());
	
	// Test 2: graph with 3 nodes
	bhk* bhk_solver2 = new bhk();
	bhk2* bhk2_solver2 = new bhk2();
	bhk3* bhk3_solver2 = new bhk3();
	int ncount2 = 3;
	int ecount2 = 3;
	int elist2[] = {0,1, 0,2, 1,2};
	int elen2[] = {3, 4, 5};
	int **distmatrix2 = distmatrix_from_elist(ncount2, ecount2, elist2, elen2);
	bhk_solver2->init(ncount2, distmatrix2);
	bhk2_solver2->init(ncount2, distmatrix2);
	bhk3_solver2->init(ncount2, distmatrix2);
	printf("Test 2:\n");
	checkValue("BHK", 12, bhk_solver2->solve());
	checkValue("BHK2", 12, bhk2_solver2->solve());
	checkValue("BHK3", 12, bhk3_solver2->solve());

	// Test 3: graph with 4 nodes
	bhk* bhk_solver3 = new bhk();
	bhk2* bhk2_solver3 = new bhk2();
	bhk3* bhk3_solver3 = new bhk3();
	int ncount3 = 4;
	int ecount3 = 6;
	int elist3[] = {0,1, 1,2, 2,3, 0,3, 0,2, 1,3};
	int elen3[] = {1,1,1,1,2,2};
	int **distmatrix3 = distmatrix_from_elist(ncount3, ecount3, elist3, elen3);
	bhk_solver3->init(ncount3, distmatrix3);
	bhk2_solver3->init(ncount3, distmatrix3);
	bhk3_solver3->init(ncount3, distmatrix3);
	printf("Test 3:\n");
	checkValue("BHK", 4, bhk_solver3->solve());
	checkValue("BHK2", 4, bhk2_solver3->solve());
	checkValue("BHK3", 4, bhk3_solver3->solve());
	
	// Test 4: graph with 6 nodes
	bhk* bhk_solver4 = new bhk();
	bhk2* bhk2_solver4 = new bhk2();
	bhk3* bhk3_solver4 = new bhk3();
	int ncount4 = 6;
	int ecount4 = 15;
	int elist4[] = {0,2, 2,5, 5,1, 1,3, 3,4, 4,0, 0,1, 0,3, 0,5, 1,2, 1,4, 2,3, 2,4, 3,5, 4,5};
	int elen4[] = {2,3,5,4,7,6, 9,8,9,8,9,8,9,8,9};
	int **distmatrix4 = distmatrix_from_elist(ncount4, ecount4, elist4, elen4);
	bhk_solver4->init(ncount4, distmatrix4);
	bhk2_solver4->init(ncount4, distmatrix4);
	bhk3_solver4->init(ncount4, distmatrix4);
	printf("Test 4:\n");
	checkValue("BHK", 27, bhk_solver4->solve());
	checkValue("BHK2", 27, bhk2_solver4->solve());
	checkValue("BHK3", 27, bhk3_solver4->solve());

	printf("Done.\n");
	return 0;
}

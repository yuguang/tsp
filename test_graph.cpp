#include "graph.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

int main (int ac, char **av)
{
  printf("Running tests...\n");
  
  graph* g = new graph();

  // Tests 1-2: An unconnected graph
  int elist1[] = {0,1, 0,2, 1,2};
  double x1[] = {0.0, 1.0, 0.0};
  g->init(x1, 3, 3, elist1);
  printf("Test 1: ");
  if (not g->is_connected()) {
    printf("PASSED\n");
  } else {
    printf("FAILED\n");
  }
  std::vector<std::vector<int> > components2;
  std::vector<int> v2_1;
  std::vector<int> v2_2;
  v2_1.push_back(0);
  v2_1.push_back(2);
  v2_2.push_back(1);
  components2.push_back(v2_1);
  components2.push_back(v2_2);
  printf("Test 2: ");
  if (components2 == g->get_components()) {
    printf("PASSED\n");
  } else {
    printf("FAILED\n");
  }

  // Tests 3-4: A connected graph
  int elist3[] = {0,1, 0,2, 1,2};
  double x3[] = {1.0, 1.0, 0.0};
  g->init(x3, 3, 3, elist3);
  printf("Test 3: ");
  if (g->is_connected()) {
    printf("PASSED\n");
  } else {
    printf("FAILED\n");
  }
  std::vector<std::vector<int> > components4;
  std::vector<int> v4_1;
  v4_1.push_back(0);
  v4_1.push_back(1);
  v4_1.push_back(2);
  components4.push_back(v4_1);
  printf("Test 4: ");
  if (components4 == g->get_components()) {
    printf("PASSED\n");
  } else {
    printf("FAILED\n");
  }

  // Tests 5-6: Test delta function
  int elist5[] = {0,1, 0,2, 0,3, 1,2, 1,3, 2,3};
  double x5[] = {1.0, 1.0, 0.0};
  g->init(x5, 4, 6, elist5);
  std::vector<int> s5;
  s5.push_back(0);
  s5.push_back(1);
  std::vector<int> delta5;
  delta5.push_back(1);
  delta5.push_back(2);
  delta5.push_back(3);
  delta5.push_back(4);
  printf("Test 5: ");
  if (delta5 == g->delta(s5)) {
    printf("PASSED\n");
  } else {
    printf("FAILED\n");
  }
  std::vector<int> s6;
  s6.push_back(0);
  s6.push_back(2);
  s6.push_back(3);
  std::vector<int> delta6;
  delta6.push_back(0);
  delta6.push_back(3);
  delta6.push_back(4);
  printf("Test 6: ");
  if (delta6 == g->delta(s6)) {
    printf("PASSED\n");
  } else {
    printf("FAILED\n");
  }

  printf("Done.\n");
  return 0;
}

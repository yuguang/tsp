#include "graph.h"
#include "tnode.h"

graph::graph() {
}

void graph::init(double * x, int ncount, int ecount, int *elist) {
  // TODO: build the graph from x
}

graph::~graph() {
}


bool graph::is_connected() {
  // TODO
  return false;
}

std::vector<std::vector<int> > graph::get_components() {
  // TODO
  std::vector<std::vector<int> > components;
  return components;
}

bool* graph::delta(int *s, int ncount, int ecount, int *elist) {
  // TODO
  bool* d = new bool[ecount];
  return d;
}

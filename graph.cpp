#include "graph.h"

graph::graph() {
}

void graph::init(double * x, int ncount, int ecount, int *elist) {

  this->ncount = ncount;
  this->ecount = ecount;
  this->elist = elist;
  this->nodes = new tnode[ncount];

  for (int i = 0; i < ecount; i++) {
    // TODO: should we have a different cutoff? like 0.0001?
    if (x[i] > 0) {
      nodes[elist[2*i]].join(&(nodes[elist[2*i+1]]));
    }
  }
}

graph::~graph() {
}


bool graph::is_connected() {
  int label1 = this->nodes[0].find_label();
  for (int i = 1; i < this->ncount; i++) {
    if (this->nodes[i].find_label() != label1) {
      return false;
    }
  }
  return true;
}

std::vector<std::vector<int> > graph::get_components() {
  // TODO
  std::vector<std::vector<int> > components;
  return components;
}

std::vector<int> graph::delta(std::vector<int> s) {
  bool* in_s = new bool[this->ncount];

  // TODO: is there a better way to do this initialization in C++?
  for (int i = 0; i < this->ncount; i++) {
    in_s[i] = false;
  }

  for (unsigned int i = 0; i < s.size(); i++) {
    in_s[s[i]] = true;
  }

  std::vector<int> d;
  for (int i = 0; i < this->ecount; i++) {
    if (in_s[this->elist[2*i]] != in_s[this->elist[2*i+1]]) {
      d.push_back(i);
    }
  }
  return d;
}

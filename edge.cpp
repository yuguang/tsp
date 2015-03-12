#include "edge.h"

edge::edge(int end1, int end2, int len, int ind) {
  this->end1 = end1;
  this->end2 = end2;
  this->len = len;
  this->ind = ind;
}

bool operator<(const edge & l, const edge & r) {
  return l.len < r.len;
}

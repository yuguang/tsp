#ifndef EDGE_H
#define EDGE_H

class edge {
  public:
    int end1;
    int end2;
    int len;
    int ind;
    edge(int end1, int end2, int len, int ind);
};

bool operator<(const edge & l, const edge & r);

#endif
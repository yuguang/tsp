#include "graph.h"
#include "edge.h"
#include <map>
#include <cstdio>
#include <iostream>

graph::graph() {
	nodes = NULL;
}

void graph::init(double * x, int ncount, int ecount, int *elist, int *elen) {
  this->ncount = ncount;
  this->ecount = ecount;
  this->elist = elist;
  this->elen = elen;
  this->nodes = new tnode[ncount];
  for (int i = 0; i < ncount; i++) {
    nodes[i].init(i);
  }
  for (int i = 0; i < ecount; i++) {
    // TODO: should we have a different cutoff? like 0.0001?
    if (x[i] > 0) {
      nodes[elist[2*i]].join(&(nodes[elist[2*i+1]]));
    }
  }
}

void graph::init(int ncount, int ecount, int *elist, int *elen) {
  this->ncount = ncount;
  this->ecount = ecount;
  this->elist = elist;
  this->elen = elen;
  this->nodes = new tnode[ncount];
  for (int i = 0; i < ncount; i++) {
    nodes[i].init(i);
  }
  for (int i = 0; i < ecount; i++) {
      nodes[elist[2*i]].join(&(nodes[elist[2*i+1]]));
  }
}

graph::~graph() {
	if( nodes ) delete [] nodes;
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
  std::vector<std::vector<int> > components;
  std::map<int,int> cMap; // maps label of node to index in components vector
  int componentCount = 0;
  for (int i = 0; i < this->ncount; i++) {
    int label = nodes[i].find_label();
    if (cMap.find(label) == cMap.end()) {
      // this label has not been seen before
      cMap[label] = componentCount;
      componentCount++;
      std::vector<int> newVector;
      newVector.push_back(i);
      components.push_back(newVector);
    } else {
      (components[cMap[label]]).push_back(i);
    }
  }
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
  delete [] in_s;
  return d;
}

std::vector<int> graph::min_spanning_tree(std::vector<int> must_include, std::vector<int> must_ignore) {
  tnode *nodes = new tnode[ncount];
  std::vector<edge> edges;
  std::vector<int> mst;

  for( int i = 0; i < ncount; i++ ) nodes[i].init(i);

  edges.reserve(ecount);
  for( int i = 0; i < ecount; i++ ) {
    edges.push_back(edge(elist[2*i], elist[2*i+1], elen[i], i));
  }

  std::sort(edges.begin(), edges.end());
  mst.reserve(ncount-1);

  for( unsigned i = 0; i < must_include.size(); i++ ) {
    if( nodes[elist[2*must_include[i]]].find_label() != nodes[elist[2*must_include[i]+1]].find_label() ) {
      mst.push_back(must_include[i]);
      nodes[elist[2*must_include[i]]].join(&nodes[elist[2*must_include[i]+1]]);
    }
  }
  
  for( int i = 0; i < ecount; i++ ) {
    if( nodes[edges[i].end1].find_label() != nodes[edges[i].end2].find_label() && find(must_ignore.begin(),must_ignore.end(),i) == must_ignore.end()) {
      for( unsigned j = 0; j < must_include.size(); j++ ) {
        if( i == must_include[j] ) {
          continue;
        }
      }
      mst.push_back(edges[i].ind);
      nodes[edges[i].end1].join(&(nodes[edges[i].end2]));
    } 
    if( (int)mst.size() == ncount-1 ) {
      break;
    }
  }

  delete [] nodes;
  return mst;
}
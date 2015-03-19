#include "bhk3.h"
#include <limits>
#include <cstdio>

bhk3::bhk3() {

}

bhk3::~bhk3() {
	
}

void bhk3::init_binomial() {
   int i, j;
   this->binomial = new int*[this->ncount];
   for (i=0; i < this->ncount; i++) {
   	  this->binomial[i] = new int[this->ncount];
      for (j=0; j < this->ncount; j++) {
         if (j == 0) this->binomial[i][j] = 1;
         else if (i == 0) this->binomial[i][j] = 0;
         else this->binomial[i][j] = this->binomial[i-1][j-1] + this->binomial[i-1][j];
      }
   }
   this->Sbase = new int[this->ncount];
   this->Sbase[0] = 0;
   for (i=1; i < this->ncount; i++)
      this->Sbase[i] = this->Sbase[i-1] + (i-1)*this->binomial[this->ncount-1][i-1];
}

int bhk3::dist(int i, int j) {
    return this->dmatrix[i][j];
}

unsigned long int bhk3::Sval(int sz, Set s, int t_indx) {
   unsigned long int loc = 0;
   int i;
   for (i=0; i<sz; i++) {
      loc += this->binomial[s[i]][i+1];
   }
   return this->Sbase[sz] + sz * loc + t_indx;
}

void bhk3::firstset(int sz, Set s) {
   int i;
   for (i=0; i<sz; i++) s[i] = i;
}

void bhk3::nextset(int sz, Set s) {
   int i;
   for (i=0; i < sz-1 && s[i]+1 == s[i+1]; i++) s[i] = i;
   s[i] = s[i]+1;
}

void bhk3::build_S() {
   int t, j, setsize, minv, v, valbase;
   Set s = new int[this->ncount];
   Set s_minus_t = new int[this->ncount];
    
   this->S = new int[(this->ncount-1)*(long int)1<<(this->ncount-2)];
   if (!(this->S)) {
      fprintf (stderr, "Error: out of memory\n");
      return;
   }
   for (this->firstset(1,s); s[0] < this->ncount-1; this->nextset(1,s))
      this->S[this->Sval(1,s,0)] = dist(s[0],ncount-1);
   for (setsize=2; setsize < this->ncount; setsize++) {
      for (this->firstset(setsize,s); s[setsize-1]<ncount-1; this->nextset(setsize,s)) {
         for (t=1; t<setsize; t++) {
            s_minus_t[t-1] = s[t];
         }
         for (t=0; t<setsize; t++) {
            valbase = this->Sval(setsize-1,s_minus_t,0);
            minv = std::numeric_limits<int>::max();
            for (j=0; j<setsize-1; j++) {
               v = this->S[valbase + j] + this->dist(s[t],s_minus_t[j]);
               if (v < minv) minv = v;
            }
            this->S[this->Sval(setsize,s,t)] = minv;
            s_minus_t[t] = s[t];
         }
      }
   }
}

void bhk3::init(int ncount, int **dmatrix) {
	this->ncount = ncount;
	this->dmatrix = dmatrix;
	this->init_binomial();
}

int bhk3::solve() {
   int t, len;
   int bestlen = std::numeric_limits<int>::max();
   Set s = new int[this->ncount];
   
   this->build_S();
   this->firstset(ncount-1,s);
   for (t=0; t<ncount-1; t++) {
      len = this->S[this->Sval(ncount-1,s,t)] + dist(s[t],ncount-1);
      if (len < bestlen) bestlen = len;
   }
	
   return bestlen;
}


// This file is from Professor Cook

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define maxN 32
#define MAXCOST 1000000
#define RANDBOX 1001

int ncount, datx[maxN], daty[maxN], distmatrix[maxN][maxN];

void dist_build()
{
    int i, j, k;
    double t1, t2;
    for (i = 0; i < ncount; i++) {
        datx[i] = random() % RANDBOX;
        daty[i] = random() % RANDBOX;
    }

    for (i = 0; i < ncount; i++) {
        for (j = 0; j < i; j++) {
            t1 = (double) (datx[i] - datx[j]);
            t2 = (double) (daty[i] - daty[j]);
            k = (int) (sqrt (t1*t1 + t2*t2) + 0.5);
            distmatrix[i][j] = distmatrix[j][i] = k;
        }
        distmatrix[i][i] = 0;
    }
}

void dist_read (char *in)
{
    FILE *fin = fopen (in, "r");
    int i, j, k;

    fscanf (fin, "%d", &ncount);
    for (i = 0; i < ncount; i++) {
        for (j = 0; j <= i; j++) {
            fscanf (fin, "%d", &k);
            distmatrix[i][j] = distmatrix[j][i] = k;
        }
    }
}

int dist(int i, int j)
{
    return distmatrix[i][j];
}

unsigned long int binomial[maxN][maxN], Sbase[maxN];
static int *S;
typedef int Set[maxN];

void init_binomial (void)
{
   int i, j;
   for (i=0; i<ncount; i++) {
      for (j=0; j<ncount; j++) {
         if (j == 0) binomial[i][j] = 1;
         else if (i == 0) binomial[i][j] = 0;
         else binomial[i][j] = binomial[i-1][j-1] + binomial[i-1][j];
      }
   }
   Sbase[0] = 0;
   for (i=1; i<ncount; i++)
      Sbase[i] = Sbase[i-1] + (i-1)*binomial[ncount-1][i-1];
}
			     
int *Sval (int sz, Set s, int t_indx) {
   unsigned long int loc = 0;
   int i;
   for (i=0; i<sz; i++) {
      loc += binomial[s[i]][i+1];
   }
   return &S[Sbase[sz] + sz * loc + t_indx];
}

void firstset (int sz, Set s)
{
   int i;
   for (i=0; i<sz; i++) s[i] = i;
}

void nextset (int sz, Set s)
{
   int i;
   for (i=0; i < sz-1 && s[i]+1 == s[i+1]; i++) s[i] = i;
   s[i] = s[i]+1;
}

void build_S () {
   int t, j, setsize, minv, v, *valbase;
   Set s, s_minus_t;
    
   S = (int *) malloc (((ncount-1)*(long int)1<<(ncount-2)) * sizeof (int));
   if (!S) {
      fprintf (stderr, "Error: out of memory\n");
      exit(1);
   }
   for (firstset(1,s); s[0]<ncount-1; nextset(1,s))
      *Sval(1,s,0) = dist(s[0],ncount-1);
   for (setsize=2; setsize<ncount; setsize++) {
      for (firstset(setsize,s); s[setsize-1]<ncount-1; nextset(setsize,s)) {
         for (t=1; t<setsize; t++) s_minus_t[t-1] = s[t];
         for (t=0; t<setsize; t++) {
            valbase = Sval(setsize-1,s_minus_t,0);
            minv = MAXCOST;
            for (j=0; j<setsize-1; j++) {
               v = valbase[j] + dist(s[t],s_minus_t[j]);
               if (v < minv) minv = v;
            }
            *Sval(setsize,s,t) = minv;
            s_minus_t[t] = s[t];
         }
      }
   }
}

int main (int ac, char **av)
{
   int t, j, setsize, last, bestlen = MAXCOST, len;
   Set s;

   if (ac == 2) {
      dist_read(av[1]);
   } else if (ac == 3) {
      ncount = atoi(av[1]);
      srandom(atoi(av[2]));
      dist_build();
   } else {
      fprintf (stderr, "Usage: %s filename   OR  %s n seed\n", av[0], av[0]);
      exit (1);
   }

   init_binomial();
   build_S();

   firstset(ncount-1,s);
   for (t=0; t<ncount-1; t++) {
      len = *Sval(ncount-1,s,t) + dist(s[t],ncount-1);
      if (len < bestlen) bestlen = len;
   }

   printf ("Best Len = %d\n", bestlen);
   last = ncount-1;
   printf ("Optimal Tour: %d", last);
   for (setsize=ncount-1; setsize>=0; --setsize) {
      for (t=0; t<setsize; t++) {
         if (bestlen == (*Sval(setsize,s,t) + dist(s[t],last))) {
            bestlen -= dist(s[t],last);
            last = s[t];
            printf (" %d", last);
            for (j=t+1; j<setsize; j++) s[j-1] = s[j];
            break;
         }
      }
   }
   printf ("\n");
   return 0;
}

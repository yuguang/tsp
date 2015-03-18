/****************************************************************************/
/*                                                                          */
/*              CO759: Model for HW2, TSP via Subtour Cuts                  */
/*              Date:  January 27, 2015                                     */
/*                                                                          */
/****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <ios>
#include <fstream>
#include <getopt.h>
#include <cmath>
#include <cplex.h>
#include <math.h>
#include "lp.h"
#include "util.h"
#include <iostream>
#include <vector>
#include "graph.h"
#include "bhk.h"
#include "bhk2.h"
#include "onetree.h"

static void usage (char *f);
static int getprob (char *fname, int *p_ncount, int *p_ecount, int **p_elist,
    int **p_elen, int ***distmatrix);
static int parseargs (int ac, char **av);
static int subtour (int ncount, int ecount, int *elist, int *elen, int *tlist);
static int euclid_edgelen (int i, int j, double *x, double *y);

static int nntour(int ncount, int ecount, int *elist, int *elen, int *tlist);
static int enumeration(int ncount, int ecount, int *elist, int *elen, int *tlist, int **distmatrix);
static void permute(int ncount, int *tour, int k, int tourlen, int **dist);
static int solve(int depth, int *bbcount, CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist);
static int connect(CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist);

static char *fname = (char *) NULL;
static int debug = 0;
static int seed = 0;
static int geometric_data = 0;
static int method = 1;
static int ncount_rand = 0;
static int ncount_rand_e = 0;
static int max_elen_rand_e = 0;
static int gridsize_rand = 100;
static int bestlen = 10000000;

int main (int ac, char **av)
{
    int rval  = 0, ncount = 0, ecount = 0;
    int *elist = (int *) NULL, *elen = (int *) NULL, *tlist = (int *) NULL;
    int **distmatrix = (int **) NULL;
    double szeit;

    seed = (int) CO759_real_zeit ();

    rval = parseargs (ac, av);
    std::ofstream log("logfile.txt", std::ios_base::app | std::ios_base::out);
    // write newline first because program is terminated if execution time exceeds limit
    log << "\n";
    log << gridsize_rand;
    log << ",";
    log << ncount_rand;
    log << ",";
    if (rval) goto CLEANUP;

	if (fname) {
		printf ("Problem name: %s\n", fname);
	}
    printf ("Seed = %d\n", seed);
    if (debug)  printf ("Debugging turned on\n");
    if (geometric_data) printf ("Geometric data\n");

    if (!fname && !ncount_rand && !ncount_rand_e) {
        printf ("Must specify a problem file, or use -k or -e for random problem\n");
        rval = 1; goto CLEANUP;
    }
    if (ncount_rand_e && !max_elen_rand_e) {
        printf ("Must specify a maximum edge length with -l when using -e\n");
        rval = 1; goto CLEANUP;
    }
    printf ("Seed = %d\n", seed);
    srandom (seed);

    if (fname) {
        printf ("Problem name: %s\n", fname);
        if (geometric_data) printf ("Geometric data\n");
    }

    rval = getprob (fname, &ncount, &ecount, &elist, &elen, &distmatrix);
    if (rval) {
        fprintf (stderr, "getprob failed\n"); goto CLEANUP;
    }

    tlist = (int *) malloc ((ncount)*sizeof (int));
    if (!tlist) {
        fprintf (stderr, "out of memory for tlist\n");
        rval = 1;  goto CLEANUP; 
    }

    szeit = CO759_zeit ();
    switch (method) {
        case 1:
            bestlen = nntour(ncount,ecount,elist,elen,tlist);
            printf ("Nearest neighbor tour: %d\n", bestlen);
            enumeration(ncount,ecount,elist,elen,tlist,distmatrix);
            break;
        case 2:
            bestlen = nntour(ncount,ecount,elist,elen,tlist);
            printf ("Nearest neighbor tour: %d\n", bestlen);
            one_tree_tsp(ncount,ecount,elist,elen,bestlen);
            break;
        case 3: {
                bhk* bhk_solver = new bhk();
                bhk_solver->init(ncount,distmatrix);
                printf("Bellman-Held-Karp 1: %d\n", bhk_solver->solve());
        	}
        	break;
        case 4:
            bestlen = nntour(ncount,ecount,elist,elen,tlist);
            printf ("Nearest neighbor tour: %d\n", bestlen);
            rval = subtour (ncount, ecount, elist, elen, tlist);
            if (rval) {
                fprintf (stderr, "subtour failed\n");
                goto CLEANUP;
            }
            break;
        case 5: {
                bhk2* bhk2_solver = new bhk2();
                bhk2_solver->init(ncount,distmatrix);
                printf("Bellman-Held-Karp 2: %d\n", bhk2_solver->solve());
            }
            break;
        default:
            break;
    }
    printf ("Running Time: %.6f seconds\n", CO759_zeit() - szeit);
    log << (CO759_zeit() - szeit);
    fflush (stdout);

CLEANUP:

    if (tlist) free (tlist);
    if (elist) free (elist);
    if (elen) free (elen);
    return rval;
}

#define LP_EPSILON 0.000001

static int subtour (int ncount, int ecount, int *elist, int *elen, int *tlist)
{
    int rval = 0, i, j, infeasible = 0;
    int bbcount = 0;
    double  obj[1], lb[1], ub[1], objval, *x = (double *) NULL;
    int     cmatbeg[1], cmatind[2];
    double  cmabestlen[2];
    CO759lp lp;

    rval = CO759lp_init (&lp);
    if (rval) { fprintf (stderr, "CO759lp_init failed\n"); goto CLEANUP; }

    rval = CO759lp_create (&lp, "subtour");
    if (rval) { fprintf (stderr, "CO759lp_create failed\n"); goto CLEANUP; }

    /* Build a row for each degree equation */

    for (i = 0; i < ncount; i++) {
        rval = CO759lp_new_row (&lp, 'E', 2.0);
        if (rval) {
            fprintf (stderr, "CO759lp_new_row failed\n"); goto CLEANUP;
        }
    }

    /* Build a column for each edge of the graph */

    cmatbeg[0] = 0;
    cmabestlen[0] = 1.0;
    cmabestlen[1] = 1.0;
    for (j = 0; j < ecount; j++) {
        obj[0]     = (double) elen[j];
        lb[0]      = 0.0;
        ub[0]      = 1.0;
        cmatind[0] = elist[2*j];
        cmatind[1] = elist[2*j+1];
        rval = CO759lp_addcols (&lp, 1 /* # of new variables */,
           2 /* # of new nonzeros */, obj, cmatbeg, cmatind, cmabestlen, lb, ub);
        if (rval) {
            fprintf (stderr, "CClp_addcols failed\n"); goto CLEANUP;
        }
    }

    rval = CO759lp_write (&lp, "subtour.lp");
    if (rval) {
        fprintf (stderr, "CO759lp_write failed\n"); goto CLEANUP;
    }

    rval = CO759lp_opt (&lp, &infeasible);
    if (rval) {
        fprintf (stderr, "CO759lp_opt failed\n"); goto CLEANUP;
    }
    if (infeasible) {
        fprintf (stderr, "LP is infeasible\n"); 
        rval = 1; goto CLEANUP;
    }

    rval = CO759lp_objval (&lp, &objval);
    if (rval) {
        fprintf (stderr, "CO759lp_objval failed 1\n"); goto CLEANUP;
    }

    printf ("Degree-Equation LP Value: %f\n", objval);
    fflush (stdout);

    x = (double *) malloc (ecount * sizeof (double));
    if (!x) {
        fprintf (stderr, "out of memory for x\n");
        rval = 1; goto CLEANUP;
    }

    rval = CO759lp_x (&lp, x);
    if (rval) {
        fprintf (stderr, "CO759lp_x failed\n"); goto CLEANUP;
    }
 
    for (i = 0, j = 0; j < ecount; j++) {
        if (x[j] > LP_EPSILON) i++;
    }

/*
    printf ("LP graph has %d edges\n", i);
    for (j = 0; j < ecount; j++) {
        if (x[j] > LP_EPSILON) {
            printf ("%d %d %f\n", elist[2*j], elist[2*j+1], x[j]);
        }
    }
    fflush (stdout);
*/
	
    solve(0, &bbcount, &lp,ncount,ecount,elist,elen,tlist);

	std::cout << "Optimal Tour " << bestlen << std::endl;

    printf ("Total Number of Search Nodes: %d\n", bbcount);
    fflush (stdout);
	
CLEANUP:
    CO759lp_free (&lp);
    if (x) free (x);
    return rval;
}
// elist - array 2*ecount
// elist[2*i] is end0 of ith edge
// elist[2*i+1] is end1 of ith edge
// elen - array ecount
// elen[i] is length of ith edge
int enumeration(int ncount, int ecount, int *elist, int *elen, int *tlist, int **distmatrix)
{
    int *tour = new int[ncount];
    for (int i = 0; i < ncount; i++) tour[i] = i;
    permute(ncount,tour,ncount-1,0,distmatrix);
    return 1;
}

#define  swap(i, j)  temp = tour[i]; tour[i] = tour[j]; tour[j] = temp;

void permute(int ncount, int *tour, int k, int tourlen, int **dist)
{
    int i, temp;
    if (tourlen >= bestlen) return;

    if (k == 1) {
        tourlen += (dist[tour[0]][tour[1]] + dist[tour[ncount-1]][tour[0]]);
        if (tourlen < bestlen) {
            bestlen = tourlen;
        }
    } else {
        for (i = 0; i < k; i++) {
            swap(i,k-1);
            permute(ncount, tour, k-1, tourlen+dist[tour[k-1]][tour[k]], dist);
            swap(i,k-1);
        }
    }
}

int nntour(int ncount, int ecount, int *elist, int *elen, int *tlist)
{
    int start = 0;
    int *marks = new int[ncount];
    int i, j, best, bestend = 0, end = start, len = 0;
    int nn_bestlen = bestlen;

    for (start = 0; start < ncount; start ++ ) {
        end = start;
        len = 0;
        for (i = 0; i < ncount; i++) marks[i] = 0;
        for (i = 1; i < ncount; i++) {
            marks[end] = 1;
            best = bestlen;
            for (j = 0; j < ecount; j++) {
                if( elist[2*j] == end ) {
                    if (marks[elist[2*j+1]] == 0 && elen[j] < best) {
                        best = elen[j]; bestend = elist[2*j+1];
                    } 
                }
                if( elist[2*j+1] == end ) {
                    if (marks[elist[2*j]] == 0 && elen[j] < best) {
                        best = elen[j]; bestend = elist[2*j];
                    } 
                }
                // if (marks[j] == 0 && elen[j] < best) {
                //     best = elen[j]; bestend = elist[2*j+1];
                // }
            }
            len += best; end = bestend;
        }

        int len_before_ret = len;
        for(i = 0; i < ecount; i++ ) {
            if( (elist[2*i] == start && elist[2*i+1] == end) || (elist[2*i+1] == start && elist[2*i] == end) ) {
                len += elen[i];
                break;
            }
        }
        if( len == len_before_ret ) { // Couldn't find returning edge on sparse graph
            len *= 2;
        }
        if( len  < nn_bestlen ) {
            nn_bestlen = len;
        }
    }

    delete [] marks;
    return nn_bestlen;}

static int solve(int depth, int *bbcount, CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist) {
	double * x = NULL;
	int i, frac = -1, rval, infeasible = 0;
	double bound;

        (*bbcount)++;
	
	rval = CO759lp_opt (lp, &infeasible);
	if (rval) {
		fprintf (stderr, "CO759lp_opt failed\n"); goto CLEANUP;
	}
	if (infeasible) {
		fprintf (stderr, "LP is infeasible\n"); 
		rval = 1; goto CLEANUP;
	}
	
	connect(lp,ncount,ecount,elist,elen,tlist);
	
	rval = CO759lp_objval (lp, &bound);
    if (rval) {
        fprintf (stderr, "CO759lp_objval failed 3\n"); goto CLEANUP;
    }

    printf ("Depth %d, BBcount: %d,  Bound: %.3f, bestlen: %d\n", depth, *bbcount, bound, bestlen);
    fflush (stdout);
	
	if( bound >= bestlen ) {
		rval = 0;
		goto CLEANUP;
	} else {
		x = new double[ecount];	
		if( !x ) {
			std::cerr << "out of memory for x" << std::endl;
			rval = 1;
			goto CLEANUP;
		}
		rval = CO759lp_x(lp, x);
		if(rval) {
			std::cerr << "CO759lp_x failed" << std::endl; goto CLEANUP;
		}
		
		for(i = 0; i < ecount; i++ ) {
			if( x[i] > LP_EPSILON && x[i] < (1-LP_EPSILON) ) {
				//std::cout << "Fractional edge " << elist[2*i] << " " << elist[2*i+1] << " " << x[i] << std::endl; 
				frac = i;
				break;
			}	
		}

		if( frac == -1 ) {
			//std::cout << "No fractional edge " << std::endl;
			//std::cout << "Setting bestlen to: " << bound << std::endl;
			bestlen = bound;
			rval = 0;
			goto CLEANUP;
		} 
		
		int cnt = 2;
		int indices[2]; indices[0] = frac; indices[1] = frac;
		char lu[2] = {'L','U'};
		double bd[2];
		
        // Change fractional edge to be 1
        bd[0] = 1.0; bd[1] = 1.0;
        CO759lp_chgbds(lp,cnt,indices,lu,bd);
        solve(depth+1,bbcount,lp,ncount,ecount,elist,elen,tlist);

		// Change fractional edge to be 0
		bd[0] = 0.0; bd[1] = 0.0;
		CO759lp_chgbds(lp,cnt,indices,lu,bd);
		solve(depth+1,bbcount,lp,ncount,ecount,elist,elen,tlist);
		
		// Change bounds back to 0, 1
		bd[0] = 0.0; bd[1] = 1.0;
		CO759lp_chgbds(lp,cnt,indices,lu,bd);
		delete [] x; x = NULL;
	}
	
CLEANUP:
	if( x ) delete [] x;

	return rval;
}

static int connect(CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist) {
	int rval, infeasible = 0;
	double * x;
	double rhs[1] = {2.0};
	char sense[1] = {'G'};
	int newnz, *rmatbeg = NULL, *rmatind = NULL;
	double *rmabestlen = NULL;
	graph support;
	
	x = new double[ecount];	
	if( !x ) {
		std::cerr << "out of memory for x" << std::endl;
		rval = 1;
		goto CLEANUP;
	}
	rval = CO759lp_x(lp, x);
	if(rval) {
		std::cerr << "CO759lp_x failed" << std::endl;
		goto CLEANUP;
	}
	
	support.init(x, ncount, ecount, elist, elen);
	
	while( !support.is_connected() ) {
		std::vector<std::vector<int> > components = support.get_components();
		// Add delta constraints to LP
		for(unsigned i = 0; i < components.size(); i++ ) {
			newnz = support.delta(components[i]).size();
			rmatbeg = new int[newnz];
			rmatind = new int[newnz];
			rmabestlen = new double[newnz];
			for(int j = 0; j < newnz; j++ ) {
				rmatbeg[j] = 0;
				rmatind[j] = support.delta(components[i])[j];
				rmabestlen[j] = 1.0;
			}
			rval = CO759lp_addrows (lp, 1, newnz, rhs, sense, rmatbeg, rmatind, rmabestlen);
			if( rval ) { std::cerr << "CO759lp_create failed" << std::endl; goto CLEANUP; }
			delete [] rmatbeg; rmatbeg = NULL;
			delete [] rmatind; rmatind = NULL;
			delete [] rmabestlen; rmabestlen = NULL;
		}
		
		rval = CO759lp_opt (lp, &infeasible);
		if (rval) {
			fprintf (stderr, "CO759lp_opt failed\n"); goto CLEANUP;
		}
		if (infeasible) {
			fprintf (stderr, "LP is infeasible\n"); 
			rval = 1; goto CLEANUP;
		}
		rval = CO759lp_x(lp, x);
		if(rval) {
			std::cerr << "CO759lp_x failed" << std::endl;
		}
		
		support.graph::~graph();
		support = graph();
		support.init(x, ncount, ecount, elist, elen);
	}

	//CO759lp_write(lp,"deltas.lp");
CLEANUP:	
	if( rmatbeg ) delete [] rmatbeg;
	if( rmatind ) delete [] rmatind;
	if( rmabestlen ) delete [] rmabestlen;
	if( x ) delete [] x;
	return rval;
}

static int getprob (char *filename, int *p_ncount, int *p_ecount, int **p_elist,
    int **p_elen, int ***p_distmatrix)
{
    FILE *f = (FILE *) NULL;
    int i, j, end1, end2, w, rval = 0, ncount, ecount;
    int *elist = (int *) NULL, *elen = (int *) NULL;
    double *x = (double *) NULL, *y = (double *) NULL;
    int **distmatrix = (int **) NULL;

    if (filename) {
        if ((f = fopen (filename, "r")) == NULL) {
    	    fprintf (stderr, "Unable to open %s for input\n",filename);
            rval = 1;  goto CLEANUP;
        }
    }

    if (filename && geometric_data == 0) {
        if (fscanf (f, "%d %d", &ncount, &ecount) != 2) {
       	    fprintf (stderr, "Input file %s has invalid format\n",filename);
            rval = 1;  goto CLEANUP;
        }

        printf ("Nodes: %d  Edges: %d\n", ncount, ecount);
        fflush (stdout);

        elist = (int *) malloc (2 * ecount * sizeof (int));
        if (!elist) {
            fprintf (stderr, "out of memory for elist\n");
            rval = 1;  goto CLEANUP;
        }

        elen = (int *) malloc (ecount * sizeof (int));
        if (!elen) {
            fprintf (stderr, "out of memory for elen\n");
            rval = 1;  goto CLEANUP;
        }
        
        distmatrix = new int* [ncount];
        for (i = 0; i < ncount; i++) {
            distmatrix[i] = new int[ncount];
            for (j = 0; j < ncount; j++) {
                distmatrix[i][j] = 0;
            }
        }

        for (i = 0; i < ecount; i++) {
    	    if (fscanf(f,"%d %d %d",&end1, &end2, &w) != 3) {
	            fprintf (stderr, "%s has invalid input format\n", filename);
                rval = 1;  goto CLEANUP;
	        }
	        elist[2*i] = end1;
	        elist[2*i+1] = end2;
	        elen[i] = w;
            distmatrix[end1][end2] = w;
            distmatrix[end2][end1] = w;
        }
    } else {
        if (filename) {
            if (fscanf (f, "%d", &ncount) != 1) {
       	        fprintf (stderr, "Input file %s has invalid format\n",filename);
                rval = 1;  goto CLEANUP;
            }
        } else if (ncount_rand) {
            ncount = ncount_rand;
        } else {
            ncount = ncount_rand_e;
        }

        x = (double *) malloc (ncount * sizeof (double));
        y = (double *) malloc (ncount * sizeof (double));
        if (!x || !y) {
            fprintf (stdout, "out of memory for x or y\n");
            rval = 1; goto CLEANUP;
        }

        if (filename) {
            for (i = 0; i < ncount; i++) {
    	        if (fscanf(f,"%lf %lf",&x[i], &y[i]) != 2) {
	            fprintf (stderr, "%s has invalid input format\n", filename);
                    rval = 1;  goto CLEANUP;
	        }
            }
        } else if (ncount_rand) {
            rval = CO759_build_xy (ncount, x, y, gridsize_rand);
            if (rval) {
                fprintf (stderr, "CO759_build_xy failed\n");
                goto CLEANUP;
            }
    
            printf ("%d\n", ncount);
            for (i = 0; i < ncount; i++) {
                printf ("%.0f %.0f\n", x[i], y[i]);
            }
            printf ("\n");
        }

        ecount = (ncount * (ncount - 1)) / 2;
        printf ("Complete graph: %d nodes, %d edges\n", ncount, ecount);

        elist = (int *) malloc (2 * ecount * sizeof (int));
        if (!elist) {
            fprintf (stderr, "out of memory for elist\n");
            rval = 1;  goto CLEANUP;
        }

        elen = (int *) malloc (ecount * sizeof (int));
        if (!elen) {
            fprintf (stderr, "out of memory for elen\n");
            rval = 1;  goto CLEANUP;
        }

        distmatrix = new int* [ncount];
        for (i = 0; i < ncount; i++) {
            distmatrix[i] = new int[ncount];
            for (j = 0; j < ncount; j++) {
                distmatrix[i][j] = 0;
            }
        }
        
        ecount = 0;
        for (i = 0; i < ncount; i++) {
            for (j = i+1; j < ncount; j++) {
                elist[2*ecount] = i;
                elist[2*ecount+1] = j;
                if (ncount_rand_e) {
                    w = (int) (random () % max_elen_rand_e + 1);
                    printf ("Edge length for %d,%d: %d\n", i, j, w);
                } else {
                    w = euclid_edgelen (i, j, x, y);
                }
                elen[ecount] = w;
                ecount++;
                distmatrix[i][j] = w;
                distmatrix[j][i] = w;
            }
        }
    }

    *p_ncount = ncount;
    *p_ecount = ecount;
    *p_elist = elist;
    *p_elen = elen;
    *p_distmatrix = distmatrix;

CLEANUP:
    if (f) fclose (f);
    if (x) free (x);
    if (y) free (y);
    return rval;
}

static int euclid_edgelen (int i, int j, double *x, double *y)
{
    double t1 = x[i] - x[j], t2 = y[i] - y[j];
    return (int) (sqrt (t1 * t1 + t2 * t2) + 0.5);
}

static int parseargs (int ac, char **av)
{
    int c;

    if (ac == 1) {
        usage (av[0]);
        return 1;
    }

    while ((c = getopt (ac, av, "b:gk:e:l:s:m:")) != EOF) {
        switch (c) {
        case 'b':
            gridsize_rand = atoi (optarg); 
            break;
        case 'g':
            geometric_data = 1;
            break;
        case 'k':
            ncount_rand = atoi (optarg);
            break;
        case 'e':
            ncount_rand_e = atoi (optarg);
            break;
        case 'l':
            max_elen_rand_e = atoi (optarg);
            break;
        case 'm':
            method = atoi (optarg);
            break;
        case 's':
            seed = atoi (optarg);
            break;
        case '?':
        default:
            usage (av[0]);
            return 1;
        }
    }

    if (optind < ac) fname = av[optind++];

    if (optind != ac) {
        usage (av[0]);
        return 1;
    }

    return 0;
}

static void usage (char *f)
{
    fprintf (stderr, "Usage: %s [-see below-] [prob_file]\n", f);
    fprintf (stderr, "   -b d  gridsize d for random problems\n");
    fprintf (stderr, "   -g    prob_file has x-y coordinates\n");
    fprintf (stderr, "   -k d  generate problem with d cities\n");
    fprintf (stderr, "   -e d  generate problem with d cities and random edge lengths\n");
    fprintf (stderr, "   -l d  maximum length of an edge when using -e is d\n");
    fprintf (stderr, "   -s d  random seed\n");
    fprintf (stderr, "   -m d  \n");
    fprintf (stderr, "         1. enumeration \n");
    fprintf (stderr, "         2. Held-Karp one tree \n");
    fprintf (stderr, "         3. Bellman-Held-Karp dynamic programming \n");
    fprintf (stderr, "         4. Branch and bound linear programming \n");
    fprintf (stderr, "         5. Bellman-Held-Karp implementation 2 \n");
}


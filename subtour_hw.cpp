/****************************************************************************/
/*                                                                          */
/*              CO759: Model for HW2, TSP via Subtour Cuts                  */
/*              Date:  January 27, 2015                                     */
/*                                                                          */
/****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <cmath>
#include <cplex.h>
#include "lp.h"
#include "util.h"
#include <iostream>
#include <vector>

static void usage (char *f);
static int getprob (char *fname, int *p_ncount, int *p_ecount, int **p_elist,
    int **p_elen);
static int parseargs (int ac, char **av);
static int subtour (int ncount, int ecount, int *elist, int *elen, int *tlist);
static int euclid_edgelen (int i, int j, double *x, double *y);

static int solve(CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist);
static int connect(CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist);

static char *fname = (char *) NULL;
static int debug = 0;
static int seed = 0;
static int geometric_data = 0;
static int TVAL = 10000000;

int main (int ac, char **av)
{
    int rval  = 0, ncount = 0, ecount = 0;
    int *elist = (int *) NULL, *elen = (int *) NULL, *tlist = (int *) NULL;
    double szeit, wt;

    seed = (int) CO759_real_zeit ();

    rval = parseargs (ac, av);
    if (rval) goto CLEANUP;

    printf ("Problem name: %s\n", fname);
    printf ("Seed = %d\n", seed);
    if (debug)  printf ("Debugging turned on\n");
    if (geometric_data) printf ("Geomtric data\n");

    rval = getprob (fname, &ncount, &ecount, &elist, &elen);
    if (rval) {
        fprintf (stderr, "getprob failed\n");
        goto CLEANUP;
    }

    tlist = (int *) malloc ((ncount)*sizeof (int));
    if (!tlist) {
        fprintf (stderr, "out of memory for tlist\n");
        rval = 1;  goto CLEANUP; 
    }

    szeit = CO759_zeit ();
    rval = subtour (ncount, ecount, elist, elen, tlist);
    if (rval) {
        fprintf (stderr, "subtour failed\n");
        goto CLEANUP;
    }
    printf ("Running Time: %.2f seconds\n", CO759_zeit() - szeit);
    fflush (stdout);

    wt = 0.0;
    printf ("Optimal Tour: %.0f\n", wt); fflush (stdout);

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
    double  obj[1], lb[1], ub[1], objval, *x = (double *) NULL;
    int     cmatbeg[1], cmatind[2];
    double  cmatval[2];
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
    cmatval[0] = 1.0;
    cmatval[1] = 1.0;
    for (j = 0; j < ecount; j++) {
        obj[0]     = (double) elen[j];
        lb[0]      = 0.0;
        ub[0]      = 1.0;
        cmatind[0] = elist[2*j];
        cmatind[1] = elist[2*j+1];
        rval = CO759lp_addcols (&lp, 1 /* # of new variables */,
           2 /* # of new nonzeros */, obj, cmatbeg, cmatind, cmatval, lb, ub);
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
        fprintf (stderr, "CO759lp_objval failed\n"); goto CLEANUP;
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

    printf ("LP graph has %d edges\n", i);
    for (j = 0; j < ecount; j++) {
        if (x[j] > LP_EPSILON) {
            printf ("%d %d %f\n", elist[2*j], elist[2*j+1], x[j]);
        }
    }
    fflush (stdout);
    TVAL = nntour(ncount,ecount,elist,elen,tlist);
    solve(&lp,ncount,ecount,elist,elen,tlist);

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
int nntour(int ncount, int ecount, int *elist, int *elen, int *tlist)
{
    int tour_len = 0;
    int start = 0;
    int marks = new double[ecount];
    int i, j, best, bestend, end = start, len = 0;

    for (i = 0; i < ncount; i++) marks[i] = 0;
    tour_len += elen[end];
    for (i = 1; i < ncount; i++) {
        marks[end] = 1;
        best = TVAL;
        for (j = 0; j < ncount; j++) {
            if (marks[j] == 0 && elen[j] < best) {
                best = elen[j]; bestend = j;
            }
        }
        len += best; end = bestend; tour_len += elen[end];
    }
    return tour_len;
}

static int solve(CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist) {
	double * x;
	int i, frac = -1, rval;
	int bound = connect(lp,ncount,ecount,elist,elen,tlist);
	if( bound >= TVAL ) {
		return 0;
	} else {
		x = new double[ecount];	
		if( !x ) {
			std::cerr << "out of memory for x" << std::endl;
			return 1;
		}
		rval = CO759lp_x(lp, x);
		if(rval) {
			std::cerr << "CO759lp_x failed" << std::endl;
		}
		
		for(i = 0; i < ecount; i++ ) {
			if( x[i] > LP_EPSILON && x[i] < (1-LP_EPSILON) ) {
				std::cout << "Fractional edge " << elist[2*i] << " " << elist[2*i+1] << " " << x[i] << std::endl; 
				frac = i;
				break;
			}	
		}

		frac = 0;
		if( frac == -1 ) {
			std::cerr << "No fractional edge found" << std::endl;
			return 2;
		} 
		
		int cnt = 2;
		int indices[2]; indices[0] = frac; indices[1] = frac;
		char lu[2] = {'L','U'};
		double bd[2];
		
		// Change fractional edge to be 0
		bd[0] = 0.0; bd[1] = 0.0;
		CO759lp_chgbds(lp,cnt,indices,lu,bd);
		//solve(lp,ncount,ecount,elist,elen,tlist);
		
		// Change fractional edge to be 1
		bd[0] = 1.0; bd[1] = 1.0;
		CO759lp_chgbds(lp,cnt,indices,lu,bd);
		//solve(lp,ncount,ecount,elist,elen,tlist);
		
		// Change bounds back to 0, 1
		bd[0] = 0.0; bd[1] = 1.0;
		CO759lp_chgbds(lp,cnt,indices,lu,bd);
	}
	
	return 0;	
}


static int connect(CO759lp * lp, int ncount, int ecount, int *elist, int *elen, int *tlist) {
	return 0;
}



static int getprob (char *filename, int *p_ncount, int *p_ecount, int **p_elist,
    int **p_elen)
{
    FILE *f = (FILE *) NULL;
    int i, j, end1, end2, w, rval = 0, ncount, ecount;
    int *elist = (int *) NULL, *elen = (int *) NULL;
    double *x = (double *) NULL, *y = (double *) NULL;

    if ((f = fopen (filename, "r")) == NULL) {
	fprintf (stderr, "Unable to open %s for input\n",filename);
        rval = 1;  goto CLEANUP;
    }

    if (geometric_data == 0) {
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

        for (i = 0; i < ecount; i++) {
    	    if (fscanf(f,"%d %d %d",&end1, &end2, &w) != 3) {
	        fprintf (stderr, "%s has invalid input format\n", filename);
                rval = 1;  goto CLEANUP;
	    }
	    elist[2*i] = end1;
	    elist[2*i+1] = end2;
	    elen[i] = w;
        }
    } else {
        if (fscanf (f, "%d", &ncount) != 1) {
       	    fprintf (stderr, "Input file %s has invalid format\n",filename);
            rval = 1;  goto CLEANUP;
        }

        x = (double *) malloc (ncount * sizeof (double));
        y = (double *) malloc (ncount * sizeof (double));
        if (!x || !y) {
            fprintf (stdout, "out of memory for x or y\n");
            rval = 1; goto CLEANUP;
        }

        for (i = 0; i < ncount; i++) {
    	    if (fscanf(f,"%lf %lf",&x[i], &y[i]) != 2) {
	        fprintf (stderr, "%s has invalid input format\n", filename);
                rval = 1;  goto CLEANUP;
	    }
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

        ecount = 0;
        for (i = 0; i < ncount; i++) {
            for (j = i+1; j < ncount; j++) {
                elist[2*ecount] = i;
                elist[2*ecount+1] = j;
                elen[ecount] = euclid_edgelen (i, j, x, y);
                ecount++;
            }
        }
    }

    *p_ncount = ncount;
    *p_ecount = ecount;
    *p_elist = elist;
    *p_elen = elen;

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

    while ((c = getopt (ac, av, "dgs:")) != EOF) {
        switch (c) {
        case 'd':
            debug = 1;
            break;
        case 'g':
            geometric_data = 1;
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

    if (optind >= ac) {
        usage (av[0]);
        return 1;
    }
    fname = av[optind++];

    return 0;
}

static void usage (char *f)
{
    fprintf (stderr, "Usage: %s prob_file\n", f);
    fprintf (stderr, "   -d    turn on debugging\n");
    fprintf (stderr, "   -g    prob_file has x-y coordinates\n");
    fprintf (stderr, "   -s d  random seed\n");
}


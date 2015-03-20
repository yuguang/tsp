/****************************************************************************/
/*                                                                          */
/*                      Utility Functions for CO759                         */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include "util.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

void timeval_print(struct timeval *tv)
{
    char buffer[30];
    time_t curtime;

    printf("%ld.%06ld", tv->tv_sec, tv->tv_usec);
    curtime = tv->tv_sec;
    strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
    printf(" = %s.%06ld\n", buffer, tv->tv_usec);
}

/* zeit function for recording times */

double CO759_zeit (void)
{
    struct rusage ru;

    getrusage (RUSAGE_SELF, &ru);

    return ((double) ru.ru_utime.tv_sec) +
           ((double) ru.ru_utime.tv_usec)/1000000.0;
}

double CO759_real_zeit (void)
{
    return (double) time (0);
}

/* function for creating a random set of points in unit square */

int CO759_build_xy (int ncount, double *xlist, double *ylist, int gridsize)
{
    int rval = 0, i, j, winner, x, y;
    int **hit = (int **) NULL, *hitcount = (int *) NULL;

    printf ("Random %d point set, gridsize = %d\n", ncount, gridsize);
    fflush (stdout);

    hit =  (int **) malloc (gridsize * sizeof (int *));
    if (!hit) {
        fprintf (stderr, "out of memory for hit\n");
        rval = 1; goto CLEANUP;
    }
    for (i = 0; i < gridsize; i++) hit[i] = (int *) NULL;

    hitcount = (int *) malloc (gridsize * sizeof (int));
    if (!hitcount) {
        fprintf (stderr, "out of memory for hitcount\n");
        rval = 1; goto CLEANUP;
    }
    for (i = 0; i < gridsize; i++) hitcount[i] = 0;

    for (i = 0; i < ncount; i++) {
        winner = 0;
        do {
            x = (int) (random () % gridsize);
            y = (int) (random () % gridsize);

            /* check to see if (x,y) is a duplicate point */

            for (j = 0; j < hitcount[x]; j++) {
                if (hit[x][j] == y) break;
            }
            if (j == hitcount[x]) {
                void *tmp_ptr = (void *) hit[x];
                tmp_ptr = realloc (tmp_ptr, (hitcount[x]+1)*sizeof (int));
                if (!tmp_ptr) {
                    fprintf (stderr, "out of member in realloc of hit\n");
                    rval = 1; goto CLEANUP;
                }
                hit[x] = (int *) tmp_ptr;
                hit[x][hitcount[x]] = y;
                hitcount[x]++;
                winner = 1;
            }
            if (!winner) {
                printf ("X"); fflush (stdout);
            }
        } while (!winner);
        xlist[i] = (double) x;
        ylist[i] = (double) y;
    }

CLEANUP:

    printf ("\n");

    if (hit) {
        for (i = 0; i < gridsize; i++) {
            if (hit[i]) free (hit[i]);
        }
        free (hit);
    }
    if (hitcount) free (hitcount);
    return rval;
}

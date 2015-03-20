/****************************************************************************/
/*                                                                          */
/*                      Utility Functions for CO759                         */
/*                                                                          */
/****************************************************************************/

#ifndef __CO759_UTIL_H
#define __CO759_UTIL_H


#define CO759_SWAP(x,y,temp) {temp = x; x = y; y = temp;}

double CO759_zeit (void);
double CO759_real_zeit (void);
void timeval_print(struct timeval *tv);
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);

int CO759_build_xy (int ncount, double *xlist, double *ylist, int gridsize);
#endif  /* __CO759_UTIL_H */

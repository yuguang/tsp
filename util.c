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


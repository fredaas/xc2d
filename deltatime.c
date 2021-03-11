#include "deltatime.h"

/* Unit reference:
 *
 * 1 s = 1000 msec (1e-3)
 *
 * 1 msec = 1000 usec (1e-6)
 *
 * 1 usec = 1000 nsec (1e-9)
 */

double dt = 0;

double dt_walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

long dt_sec(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec;
}

long dt_msec(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_usec;
}

void dt_update(void)
{
    static double time = 0;
    double walltime = dt_walltime();
    if (time == 0)
    {
        time = walltime;
        dt = 0;
    }
    double value = time;
    time = walltime;
    dt = walltime - value;
}

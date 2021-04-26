#include "vec2.h"

/* Returns the normalized unit vector of 'vec' */
void vec2_norm(double vec[2], double uvec[2])
{
    double size = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
    uvec[0] = vec[0] / size;
    uvec[1] = vec[1] / size;
}


/* Returns the size of 'vec' */
double vec2_size(double vec[2])
{
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
}

/* Returns the vector 'r1' representing the shortest distance from 'r0' to
   'rect' */
void vec2_rxdelta(Rect *rect, double x0, double y0, double *x1, double *y1)
{
    *x1 = max(rect->cx, min(rect->cx + rect->size, x0));
    *y1 = max(rect->cy, min(rect->cy + rect->size, y0));
}

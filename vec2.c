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

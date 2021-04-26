#ifndef VEC2_H
#define VEC2_H

#include "utils.h"
#include "rect.h"

void vec2_norm(double vec[2], double uvec[2]);
double vec2_size(double vec[2]);
void vec2_rxdelta(Rect *rect, double x0, double y0, double *x1, double *y1);

#endif /* VEC2_H */

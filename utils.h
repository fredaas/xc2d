#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "deltatime.h"

/* Rotate (x, y) around (0, 0) by r radians */
#define rotate_x(x, y, r) (x * cos(r) - y * sin(r))
#define rotate_y(x, y, r) (x * sin(r) + y * cos(r))

/* Extract r bits from b starting at s: b[s, s + r) */
#define bitrange(b, s, r) ((b >> s) & ((1 << r) - 1))

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define PI (double)3.141592654

#define rand_float(a, b) (a) + (rand() / (float)RAND_MAX) * ((b) - (a))

#define msec(sec) (double)((sec) * 1.0e+3)
#define usec(sec) (double)((sec) * 1.0e+6)

double walltime(void);

#endif /* UTILS_H */

#ifndef CIRCLE_H
#define CIRCLE_H

#include "utils.h"

typedef struct Circle Circle;

struct Circle {
    double cx;   /* Center position x */
    double cy;   /* Center position y */
    double r;    /* Direction */
    double size; /* Radius */
    double *points;
    int H; /* Number of (x, y) points */

    void (*draw)(Circle *self);
    void (*turn_left)(Circle *self);
    void (*turn_right)(Circle *self);
    void (*move_forward)(Circle *self);
    void (*move_backward)(Circle *self);
};

Circle *new_circle(double cx, double cy, double size);

#endif /* CIRCLE_H */
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
};

Circle *new_circle(double cx, double cy, double size);
void circle_create_shape(Circle *self);
void circle_draw(Circle *self);
void circle_turn_left(Circle *self);
void circle_turn_right(Circle *self);
void circle_move_forward(Circle *self);
void circle_move_backward(Circle *self);

#endif /* CIRCLE_H */
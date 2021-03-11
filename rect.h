#ifndef RECT_H
#define RECT_H

#include "utils.h"

typedef struct Rect Rect;

struct Rect {
    double cx;   /* Position x (upper left) */
    double cy;   /* Position y (upper left) */
    double size;
    double points[8];
    double r;
    double v;
};

Rect *new_rect(double cx, double cy, double size);
void rect_create_shape(Rect *self);
void rect_draw(Rect *self);
void rect_move_left(Rect *self);
void rect_move_right(Rect *self);
void rect_move_forward(Rect *self);
void rect_move_backward(Rect *self);
void rect_minpoint(Rect *self, double x, double y, double *nx, double *ny);

#endif /* RECT_H */
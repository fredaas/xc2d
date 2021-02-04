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

    void (*draw)(Rect *self);
    void (*move_left)(Rect *self);
    void (*move_right)(Rect *self);
    void (*move_forward)(Rect *self);
    void (*move_backward)(Rect *self);
    void (*minpoint)(Rect *self, double x, double y, double *nx, double *ny);
};

Rect *new_rect(double cx, double cy, double size);

#endif /* RECT_H */
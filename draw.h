#ifndef DRAW_H
#define DRAW_H

#include "utils.h"
#include "circle.h"
#include "rect.h"
#include "vec2.h"

void draw_color(int hex);
void draw_square(double x, double y, double size, int hex);
void draw_circle(double x, double y, double size, int hex);
void draw_circle_ray(Circle *circle, Rect *rect);
void draw_rect_ray(Rect *r1, Rect *r2);
void draw_rect_normal(Rect *r1, Rect *r2);

#endif /* DRAW_H */

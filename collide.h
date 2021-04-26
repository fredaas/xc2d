#ifndef COLLIDE_H
#define COLLIDE_H

#include "vec2.h"
#include "circle.h"
#include "rect.h"

void collide_rr(Rect *r1, Rect *r2);
void collide_cr(Circle *circle, Rect *rect);
void collide_cc(Circle *c1, Circle *c2);

#endif /* COLLIDE_H */

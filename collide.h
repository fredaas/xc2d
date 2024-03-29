#ifndef COLLIDE_H
#define COLLIDE_H

#include "vec2.h"
#include "circle.h"
#include "rect.h"
#include "vec2.h"

void collide_rr(Rect *r1, Rect *r2);
void collide_cr(Circle *circle, Rect *rect);
void collide_cc(Circle *c1, Circle *c2);
void collide_rc(Rect *rect, Circle *circle);

#endif /* COLLIDE_H */

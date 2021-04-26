#include "collide.h"

/* Resolves rect-rect collosions where 'r2' is a static structure */
void collide_rr(Rect *r1, Rect *r2)
{
    double s1 = r1->size;
    double s2 = r2->size;
    double x1 = r1->cx;
    double y1 = r1->cy;
    double x2 = r2->cx;
    double y2 = r2->cy;

    double cx1 = x1 + s1 / 2;
    double cy1 = y1 + s1 / 2;

    double nx, ny;
    vec2_rxdelta(r2, cx1, cy1, &nx, &ny);

    double vec[2] = { nx - cx1, ny - cy1 };

    if (fabs(vec[0]) > fabs(vec[1]))
    {
        if (vec[0] < 0)
        {
            if (x1 < (x2 + s2))
                r1->cx += (x2 + s2) - x1;
        }
        else
        {
            if ((x1 + s1) > x2)
                r1->cx -= (x1 + s1) - x2;
        }
    }
    else if (fabs(vec[1]) > fabs(vec[0]))
    {
        if (vec[1] > 0)
        {
            if ((y1 + s1) > y2)
                r1->cy -= (y1 + s1) - y2;
        }
        else
        {
            if (y1 < (y2 + s2))
                r1->cy += (y2 + s2) - y1;
        }
    }
}

/* Resolves circle-rect collisions where 'r' is a static structure */
void collide_cr(Circle *circle, Rect *rect)
{
    double nx, ny;
    vec2_rxdelta(rect, circle->cx, circle->cy, &nx, &ny);

    double vec[2] = { nx - circle->cx, ny - circle->cy };
    double overlap = circle->size - vec2_size(vec);

    if (overlap > 0.0)
    {
        double uvec[2] = { 0.0, 0.0 };
        vec2_norm(vec, uvec);
        circle->cx -= uvec[0] * overlap;
        circle->cy -= uvec[1] * overlap;
    }
}

void collide_rc(Rect *rect, Circle *circle)
{
    double nx, ny;
    vec2_rxdelta(rect, circle->cx, circle->cy, &nx, &ny);

    double vec[2] = { nx - circle->cx, ny - circle->cy };
    double overlap = circle->size - vec2_size(vec);

    if (overlap > 0.0)
    {
        double uvec[2] = { 0.0, 0.0 };
        vec2_norm(vec, uvec);
        rect->cx += uvec[0] * overlap;
        rect->cy += uvec[1] * overlap;
    }
}

/* Resolves circle-circle collisions where 'c2' is a static structure */
void collide_cc(Circle *c1, Circle *c2)
{
    double vec[2] = { c2->cx - c1->cx, c2->cy - c1->cy };
    double overlap = c1->size + c2->size - vec2_size(vec);
    if (overlap > 0.0)
    {
        double uvec[2] = { 0.0, 0.0 };
        vec2_norm(vec, uvec);
        c1->cx -= uvec[0] * overlap;
        c1->cy -= uvec[1] * overlap;
    }
}

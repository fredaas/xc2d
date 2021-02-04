#include "rect.h"

static void create_shape(Rect *self);
static void draw(Rect *self);
static void move_left(Rect *self);
static void move_right(Rect *self);
static void move_forward(Rect *self);
static void move_backward(Rect *self);
static void minpoint(Rect *self, double x, double y, double *nx, double *ny);

Rect *new_rect(double cx, double cy, double size)
{
    Rect *self = (Rect *)malloc(sizeof(Rect));
    self->cx = cx;
    self->cy = cy;
    self->size = size;

    create_shape(self);

    self->draw = draw;
    self->move_left = move_left;
    self->move_right = move_right;
    self->move_forward = move_forward;
    self->move_backward = move_backward;
    self->minpoint = minpoint;

    return self;
}

static void create_shape(Rect *self)
{
    double size = self->size;
    double points[8] = {
        0, 0,
        0, size,
        size, size,
        size, 0
    };

    memcpy(self->points, points, sizeof(double) * 8);
}

static void move_left(Rect *self)
{
    self->cx -= 0.5;
}
static void move_right(Rect *self)
{
    self->cx += 0.5;
}
static void move_forward(Rect *self)
{
    self->cy += 0.5;
}
static void move_backward(Rect *self)
{
    self->cy -= 0.5;
}

static void minpoint(Rect *self, double x, double y, double *nx, double *ny)
{
    *nx = max(self->cx, min(self->cx + self->size, x));
    *ny = max(self->cy, min(self->cy + self->size, y));
}

static void draw(Rect *self)
{
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 4; i++)
    {
        int x = i * 2;
        int y = i * 2 + 1;
        int tx = (((i + 1) * 2) % (4 * 2));
        int ty = (((i + 1) * 2) % (4 * 2)) + 1;
        glVertex2f(self->cx + self->points[x], self->cy + self->points[y]);
        glVertex2f(self->cx + self->points[tx], self->cy + self->points[ty]);
    }
    glEnd();
}

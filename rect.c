#include "rect.h"

Rect *new_rect(double cx, double cy, double size)
{
    Rect *self = (Rect *)malloc(sizeof(Rect));
    self->cx = cx;
    self->cy = cy;
    self->size = size;
    self->v = size * 8;

    rect_create_shape(self);

    return self;
}

void rect_create_shape(Rect *self)
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

void rect_move_left(Rect *self)
{
    self->cx -= self->v * dt;
}
void rect_move_right(Rect *self)
{
    self->cx += self->v * dt;
}
void rect_move_forward(Rect *self)
{
    self->cy += self->v * dt;
}
void rect_move_backward(Rect *self)
{
    self->cy -= self->v * dt;
}

void rect_draw(Rect *self)
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

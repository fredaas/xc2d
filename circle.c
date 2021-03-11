#include "circle.h"

Circle *new_circle(double cx, double cy, double size)
{
    Circle *self = (Circle *)malloc(sizeof(Circle));
    self->r = PI / 2;
    self->cx = cx;
    self->cy = cy;
    self->size = size;
    self->H = 15;
    self->v = size * 2 * 4;
    self->w = size / 4;

    circle_create_shape(self);

    return self;
}

void circle_create_shape(Circle *self)
{
    double delta = 2 * PI / self->H;
    double rad = 0.0;
    self->points = (double *)malloc(2 * self->H * sizeof(double));
    for (int i = 0; i < self->H; i++)
    {
        double x = cos(rad) * self->size;
        double y = sin(rad) * self->size;
        self->points[i * 2] = x;
        self->points[i * 2 + 1] = y;
        rad += delta;
    }
}

void circle_draw(Circle *self)
{
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glLineWidth(1.0f);

    glBegin(GL_LINES);

    /* Draw shape */
    for (int i = 0; i < self->H; i++)
    {
        double x = self->points[i * 2];
        double y = self->points[i * 2 + 1];
        /* % H * 2 is needed to connect the first and last point */
        double tx = self->points[((i + 1) * 2) % (self->H * 2)];
        double ty = self->points[((i + 1) * 2) % (self->H * 2) + 1];
        glVertex2f(self->cx + x, self->cy + y);
        glVertex2f(self->cx + tx, self->cy + ty);
    }

    /* Draw orientation */
    glVertex2f(self->cx, self->cy);
    glVertex2f(
        self->cx + cos(self->r) * self->size,
        self->cy + sin(self->r) * self->size
    );
    glEnd();
}

void circle_rotate(Circle *self, double delta)
{
    for (int i = 0; i < self->H; i++)
    {
        double x = self->points[i * 2];
        double y = self->points[i * 2 + 1];
        self->points[i * 2] = rotate_x(x, y, delta);
        self->points[i * 2 + 1] = rotate_y(x, y, delta);
    }

    self->r += delta;

    if (self->r > 2 * PI)
        self->r -= 2 * PI;
    else if (self->r < 0)
        self->r += 2 * PI;
}

void circle_turn_left(Circle *self)
{
    circle_rotate(self, dt * self->w);
}

void circle_turn_right(Circle *self)
{
    circle_rotate(self, dt * -self->w);
}

void circle_move_forward(Circle *self)
{
    self->cx += cos(self->r) * self->v * dt;
    self->cy += sin(self->r) * self->v * dt;
}

void circle_move_backward(Circle *self)
{
    self->cx -= cos(self->r) * self->v * dt;
    self->cy -= sin(self->r) * self->v * dt;
}

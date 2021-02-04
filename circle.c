#include "circle.h"

static void create_shape(Circle *self);
static void draw(Circle *self);
static void turn_left(Circle *self);
static void turn_right(Circle *self);
static void move_forward(Circle *self);
static void move_backward(Circle *self);

Circle *new_circle(double cx, double cy, double size)
{
    Circle *self = (Circle *)malloc(sizeof(Circle));
    self->r = PI / 2;
    self->cx = cx;
    self->cy = cy;
    self->size = size;
    self->H = 15;

    create_shape(self);

    self->draw = draw;
    self->turn_left = turn_left;
    self->turn_right = turn_right;
    self->move_forward = move_forward;
    self->move_backward = move_backward;

    return self;
}

static void create_shape(Circle *self)
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

static void draw(Circle *self)
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

static void rotate(Circle *self, double delta)
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

static void turn_left(Circle *self)
{
    rotate(self, 0.001);
}

static void turn_right(Circle *self)
{
    rotate(self, -0.001);
}

static void move_forward(Circle *self)
{
    self->cx += cos(self->r) * 0.1;
    self->cy += sin(self->r) * 0.1;
}

static void move_backward(Circle *self)
{
    self->cx -= cos(self->r) * 0.1;
    self->cy -= sin(self->r) * 0.1;
}

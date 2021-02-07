#include "draw.h"

void draw_color(int hex)
{
    glColor4f(
        biso(hex, 8, 24) / 256.0,
        biso(hex, 8, 16) / 256.0,
        biso(hex, 8, 8) / 256.0,
        biso(hex, 8, 0) / 256.0
    );
}

void draw_square(double x, double y, double size, int hex)
{
    double points[8] = {
        x, y,
        x, y + size,
        x + size, y + size,
        x + size, y
    };
    draw_color(hex);
    glBegin(GL_LINES);
    for (int i = 0; i < 4; i++)
    {
        int x = i * 2;
        int y = i * 2 + 1;
        int tx = (((i + 1) * 2) % (4 * 2));
        int ty = (((i + 1) * 2) % (4 * 2)) + 1;
        glVertex2f(points[x], points[y]);
        glVertex2f(points[tx], points[ty]);
    }
    glEnd();
}

void draw_circle(double x, double y, double size, int hex)
{
    int H = 15;
    double delta = PI * 2 / H;
    double rad = 0;
    draw_color(hex);
    glBegin(GL_LINES);
    for (int i = 0; i < H; i++)
    {
        glVertex2f(x + cos(rad) * size, y + sin(rad) * size);
        glVertex2f(x + cos(rad + delta) * size, y + sin(rad + delta) * size);
        rad += delta;
    }
    glEnd();
}

void draw_circle_ray(Circle *circle, Rect *rect)
{
    double nx, ny;
    rect_minpoint(rect, circle->cx, circle->cy, &nx, &ny);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(nx, ny);
    glVertex2f(circle->cx , circle->cy);
    glEnd();
}

void draw_rect_ray(Rect *r1, Rect *r2)
{
    double size = r1->size;
    double x = r1->cx + size / 2;
    double y = r1->cy + size / 2;
    double nx, ny;
    rect_minpoint(r2, x, y, &nx, &ny);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(nx, ny);
    glVertex2f(x, y);
    glEnd();
}

void draw_rect_normal(Rect *r1, Rect *r2)
{
    double s1 = r1->size;
    double x1 = r1->cx;
    double y1 = r1->cy;

    double cx1 = x1 + s1 / 2;
    double cy1 = y1 + s1 / 2;

    double nx, ny;
    rect_minpoint(r2, cx1, cy1, &nx, &ny);

    double vec[2] = { nx - cx1, ny - cy1 };

    glColor4f(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);

    if (fabs(vec[0]) > fabs(vec[1]))
    {
        if (vec[0] < 0)
        {
            glVertex2f(nx, ny);
            glVertex2f(nx + s1, ny);
        }
        else
        {
            glVertex2f(nx, ny);
            glVertex2f(nx - s1, ny);
        }
    }
    else if (fabs(vec[1]) > fabs(vec[0]))
    {
        if (vec[1] > 0)
        {
            glVertex2f(nx, ny);
            glVertex2f(nx, ny - s1);
        }
        else
        {
            glVertex2f(nx, ny);
            glVertex2f(nx, ny + s1);
        }
    }

    glEnd();
}

#include "utils.h"
#include "circle.h"
#include "rect.h"
#include "vec2.h"
#include "window.h"
#include "world.h"
#include "draw.h"

#define FPS 60
#define UPDATE_RATE 1000 / (float)FPS

Window *window = NULL;
World *world = NULL;
Circle *p_circle = NULL;
Rect *p_rect = NULL;

double walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + (double)t.tv_usec * 1.0e-06);
}

void collide_rect_rect(Rect *r1, Rect *r2)
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
    rect_minpoint(r2, cx1, cy1, &nx, &ny);

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

int collide_circle_rect(Circle *circle, Rect *rect)
{
    double nx, ny;
    rect_minpoint(rect, circle->cx, circle->cy, &nx, &ny);

    double vec[2] = { nx - circle->cx, ny - circle->cy };
    double overlap = circle->size - vec2_size(vec);

    if (overlap > 0.0)
    {
        double uvec[2] = { 0.0, 0.0 };
        vec2_norm(vec, uvec);
        circle->cx -= uvec[0] * overlap;
        circle->cy -= uvec[1] * overlap;
        return 1;
    }

    return 0;
}

void world_resolve_circle_collision(void)
{
    int x, y;
    world_pos2cell(world, p_circle->cx, p_circle->cy, &x, &y);

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            Rect *rect = world_cell(world, x + dx, y + dy);
            if (rect != NULL)
            {
                collide_circle_rect(p_circle, rect);
                draw_circle_ray(p_circle, rect);
            }
        }
    }
}

void world_resolve_rect_collision(void)
{
    int x, y;
    world_pos2cell(world,
        p_rect->cx + p_rect->size / 2,
        p_rect->cy + p_rect->size / 2, &x, &y);

    /* Center */
    for (int dy = -1; dy <= 1; dy++)
    {
        Rect *rect = world_cell(world, x, y + dy);
        if (rect != NULL)
        {
            draw_rect_ray(p_rect, rect);
            draw_rect_normal(p_rect, rect);
            collide_rect_rect(p_rect, rect);
        }
    }
    /* Left */
    for (int dy = -1; dy <= 1; dy++)
    {
        Rect *rect = world_cell(world, x - 1, y + dy);
        if (rect != NULL)
        {
            draw_rect_ray(p_rect, rect);
            draw_rect_normal(p_rect, rect);
            collide_rect_rect(p_rect, rect);
        }
    }
    /* Right */
    for (int dy = -1; dy <= 1; dy++)
    {
        Rect *rect = world_cell(world, x + 1, y + dy);
        if (rect != NULL)
        {
            draw_rect_ray(p_rect, rect);
            draw_rect_normal(p_rect, rect);
            collide_rect_rect(p_rect, rect);
        }
    }
}

void init_circle(void)
{
    double size = world->cell_size / 2;
    p_circle = new_circle(
        (window->width  - size) / 2,
        (window->height - size) / 2, size);
}

void update_circle(void)
{
    if (is_key_down(GLFW_KEY_J))
        circle_turn_left(p_circle);
    if (is_key_down(GLFW_KEY_L))
        circle_turn_right(p_circle);
    if (is_key_down(GLFW_KEY_I))
        circle_move_forward(p_circle);
    if (is_key_down(GLFW_KEY_K))
        circle_move_backward(p_circle);
    circle_draw(p_circle);
}

void init_rect(void)
{
    double size = world->cell_size;
    p_rect = new_rect(
        (window->width  - size) / 2,
        (window->height - size) / 2, size);
}

void update_rect(void)
{
    if (is_key_down(GLFW_KEY_A))
        rect_move_left(p_rect);
    if (is_key_down(GLFW_KEY_D))
        rect_move_right(p_rect);
    if (is_key_down(GLFW_KEY_W))
        rect_move_forward(p_rect);
    if (is_key_down(GLFW_KEY_S))
        rect_move_backward(p_rect);
    rect_draw(p_rect);
}

int main(int argc, char **argv)
{
    srand(42);

    window = new_window(1500, 1000);
    window_center(window->view);

    world = new_world("worlds/world1.txt");

    init_rect();
    init_circle();

    while (!glfwWindowShouldClose(window->view))
    {
        glfwGetFramebufferSize(window->view, &window->width, &window->height);
        glViewport(0, 0, window->width, window->height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, window->width, 0, window->height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world_update_pan(world);
        world_update_scale(world);
        glTranslated(world->x, world->y, 0);

        update_rect();
        update_circle();

        world_resolve_rect_collision();
        world_resolve_circle_collision();

        if (is_mouse_down(MOUSE_LEFT))
        {
            double mx, my, tx, ty;
            window_mouse_pos(&mx, &my);
            world_screen2world(world, mx, my, &tx, &ty);
            draw_circle(tx, ty, 10, 0xff0000ff);
        }

        world_draw(world);

        world_draw_region(
            world,
            p_circle->cx,
            p_circle->cy,
            3, 0xffcb67aa
        );

        world_draw_region(
            world,
            p_rect->cx + p_rect->size / 2,
            p_rect->cy + p_rect->size / 2,
            3, 0x67ffefaa
        );

        if (is_key_pressed(GLFW_KEY_Q))
            glfwSetWindowShouldClose(window->view, GLFW_TRUE);

        glfwSwapBuffers(window->view);
        glfwPollEvents();
    }

    glfwDestroyWindow(window->view);
    glfwTerminate();

    return 0;
}

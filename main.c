#include "circle.h"
#include "rect.h"
#include "window.h"
#include "world.h"
#include "draw.h"
#include "deltatime.h"
#include "collide.h"

Window *window = NULL;
World *world = NULL;
Circle *p_circle = NULL;
Rect *p_rect = NULL;

void resolve_circle(Cell *cell)
{
    if (cell == NULL)
        return;

    switch (cell->type)
    {
    case BLOCK_RECT:
        {
            Rect *rect = (Rect *)cell->value;
            collide_cr(p_circle, rect);
            draw_circle_ray(p_circle, rect);
            break;
        }
    case BLOCK_CIRCLE:
        {
            Circle *circle = (Circle *)cell->value;
            collide_cc(p_circle, circle);
            break;
        }
    }
}

void world_resolve_circle_collision(void)
{
    int x, y;
    world_pos2cell(world, p_circle->cx, p_circle->cy, &x, &y);

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            Cell *cell = world_cell(world, x + dx, y + dy);
            resolve_circle(cell);
        }
    }
}

void resolve_rect(Cell *cell)
{
    if (cell == NULL)
        return;

    switch (cell->type)
    {
    case BLOCK_RECT:
        {
            Rect *rect = (Rect *)cell->value;
            collide_rr(p_rect, rect);
            draw_rect_ray(p_rect, rect);
            break;
        }
    case BLOCK_CIRCLE:
        {
            Circle *circle = (Circle *)cell->value;
            collide_rc(p_rect, circle);
            draw_circle_ray(circle, p_rect);
            break;
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
        Cell *cell = world_cell(world, x, y + dy);
        resolve_rect(cell);
    }
    /* Left */
    for (int dy = -1; dy <= 1; dy++)
    {
        Cell *cell = world_cell(world, x - 1, y + dy);
        resolve_rect(cell);
    }
    /* Right */
    for (int dy = -1; dy <= 1; dy++)
    {
        Cell *cell = world_cell(world, x + 1, y + dy);
        resolve_rect(cell);
    }
}

void init_circle(void)
{
    double size = world->cell_size / 2;
    p_circle = new_circle(
        (window->width - size) / 2,
        (window->height - size) / 2, size
    );
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
        (window->width - size) / 2,
        (window->height - size) / 2, size
    );
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

int zoom_state = 0;

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

        dt_update();

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

        if (is_key_pressed(GLFW_KEY_Z))
            zoom_state = 1;

        double zoom_target = window_zoom();

        switch (zoom_state)
        {
        case 0:
            break;
        case 1:
            if (zoom_target >= 1)
                zoom_target = 0.25;
            else if (zoom_target < 1)
                zoom_target = 1.75;
            window_zoom_target(zoom_target);
            zoom_state = 2;
            break;
        case 2:
            if (!window_update_zoom())
                zoom_state = 0;
            break;
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

        window_update_keys();

        glfwSwapBuffers(window->view);
        glfwPollEvents();
    }

    glfwDestroyWindow(window->view);
    glfwTerminate();

    return 0;
}

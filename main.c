#include "utils.h"
#include "circle.h"
#include "rect.h"
#include "vec2.h"
#include "window.h"

#define FPS 60
#define UPDATE_RATE 1000 / (float)FPS

int num_spaceships = 32;

int world_w  = 1500;
int world_h  = 1500;

Rect **rects = NULL;

Window *window = NULL;

int n_rects = 0;

double walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + (double)t.tv_usec * 1.0e-06);
}

typedef struct World World;

struct World {
    int n_cells_x;
    int n_cells_y;
    int cell_size;
    Rect **cells;
};

World world;

Circle *p_circle = NULL;
Rect *p_rect = NULL;

void draw_color(int hex)
{
    glColor4f(
        biso(hex, 8, 24) / 256.0,
        biso(hex, 8, 16) / 256.0,
        biso(hex, 8, 8) / 256.0,
        biso(hex, 8, 0) / 256.0
    );
}

/* Returns the file dimensions (rows, cols) of fp */
void fsize(FILE *fp, int *nx, int *ny)
{
    char c;
    int cx = 0;
    int n_cells_y = 0;
    int n_cells_x = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        cx++;
        if (c == '\n')
        {
            cx = 0;
            n_cells_y++;
        }
        n_cells_x = max(n_cells_x, cx);
    }
    rewind(fp);
    *nx = n_cells_x;
    *ny = n_cells_y;
}

/* Returns the cell point (cx, cy) of the corresponding world point (x, y) */
void world_cell2i(double x, double y, int *cx, int *cy)
{
    double cell_size = world.cell_size;
    *cx = floor(x / cell_size);
    *cy = floor(y / cell_size);
}

/* Returns the world point (x, y) of the corresponding cell index i */
void world_cell2f(int i, double *x, double *y)
{
    *x = (i % world.n_cells_x) * world.cell_size;
    *y = (i / world.n_cells_x) * world.cell_size;
}

void world_init(char *path)
{
    FILE *fp = fopen(path, "r");

    fsize(fp, &world.n_cells_x, &world.n_cells_y);

    world.cell_size = 30;

    /* Create rects from file */
    Rect *rect_buff[256];
    char c;
    int ny = 0, nx = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n')
        {
            nx = 0;
            ny++;
        }
        else if (c == ' ')
        {
            nx++;
        }
        else if (c == '#')
        {
            double x = nx;
            double y = world.n_cells_y - ny - 1;
            double size = world.cell_size;
            rect_buff[n_rects] = new_rect(x * size, y * size, size);
            n_rects++;
            nx++;
        }
    }

    /* Initialize all world cells to NULL */
    world.cells = (Rect **)realloc(
        rects, world.n_cells_x * world.n_cells_y * sizeof(Rect *));
    for (int i = 0; i < world.n_cells_x * world.n_cells_y; i++)
        world.cells[i] = NULL;

    /* Copy rects to global array */
    rects = (Rect **)realloc(rects, n_rects * sizeof(Rect *));
    memcpy(rects, rect_buff, n_rects * sizeof(Rect *));

    /* Copy rects to world cells */
    for (int i = 0; i < n_rects; i++)
    {
        Rect *rect = rects[i];
        int x, y;
        world_cell2i(rect->cx, rect->cy, &x, &y);
        world.cells[y * world.n_cells_x  + x] = rect;
    }
}

void draw_square(double x, double y, double size)
{
    double points[8] = {
        x, y,
        x, y + size,
        x + size, y + size,
        x + size, y
    };
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

void world_draw_region(double x, double y, int hex)
{
    int dim = 3;
    int n_lines = dim - 1;
    int size = world.cell_size * dim;

    double cx = x - size / 2;
    double cy = y - size / 2;

    draw_color(hex);
    draw_square(cx, cy, size);
    glBegin(GL_LINES);
    for (int i = 1; i <= n_lines; i++)
    {
        glVertex2f(cx + i * world.cell_size, y - size / 2);
        glVertex2f(cx + i * world.cell_size, y + size / 2);
    }
    for (int i = 1; i <= n_lines; i++)
    {
        glVertex2f(x - size / 2, cy + i * world.cell_size);
        glVertex2f(x + size / 2, cy + i * world.cell_size);
    }
    glEnd();
}

void world_draw(void)
{
    for (int i = 0; i < n_rects; i++)
    {
        Rect *rect = rects[i];
        rect_draw(rect);
    }
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
            glVertex2f(nx + world.cell_size, ny);
        }
        else
        {
            glVertex2f(nx, ny);
            glVertex2f(nx - world.cell_size, ny);
        }
    }
    else if (fabs(vec[1]) > fabs(vec[0]))
    {
        if (vec[1] > 0)
        {
            glVertex2f(nx, ny);
            glVertex2f(nx, ny - world.cell_size);
        }
        else
        {
            glVertex2f(nx, ny);
            glVertex2f(nx, ny + world.cell_size);
        }
    }

    glEnd();
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

void draw_circle(double x, double y, double size)
{
    int H = 15;
    double delta = PI * 2 / H;
    double rad = 0;
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    for (int i = 0; i < H; i++)
    {
        glVertex2f(x + cos(rad) * size, x + sin(rad) * size);
        glVertex2f(x + cos(rad + delta) * size, x + sin(rad + delta) * size);
        rad += delta;
    }
    glEnd();
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

Rect *cell(int x, int y)
{
    if ((x < 0) || (x >= world.n_cells_x) || (y < 0) || (y >= world.n_cells_y))
        return NULL;
    return world.cells[y * world.n_cells_x + x];
}

void world_resolve_circle_collision(void)
{
    int x, y;
    world_cell2i(p_circle->cx, p_circle->cy, &x, &y);

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            Rect *rect = cell(x + dx, y + dy);
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
    world_cell2i(
        p_rect->cx + p_rect->size / 2,
        p_rect->cy + p_rect->size / 2, &x, &y);

    /* Center */
    for (int dy = -1; dy <= 1; dy++)
    {
        Rect *rect = cell(x, y + dy);
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
        Rect *rect = cell(x - 1, y + dy);
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
        Rect *rect = cell(x + 1, y + dy);
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
    double size = world.cell_size / 2;
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
    double size = world.cell_size;
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

    world_init("worlds/world1.txt");

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

        world_draw();

        world_draw_region(p_circle->cx, p_circle->cy, 0xffcb67aa);
        world_draw_region(
            p_rect->cx + p_rect->size / 2,
            p_rect->cy + p_rect->size / 2, 0x67ffefaa);

        update_rect();
        update_circle();

        world_resolve_rect_collision();
        world_resolve_circle_collision();

        if (is_key_pressed(GLFW_KEY_Q))
            glfwSetWindowShouldClose(window->view, GLFW_TRUE);

        glfwSwapBuffers(window->view);
        glfwPollEvents();
    }

    glfwDestroyWindow(window->view);
    glfwTerminate();

    return 0;
}

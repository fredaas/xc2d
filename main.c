#include "utils.h"
#include "circle.h"
#include "rect.h"

#define FPS 60
#define UPDATE_RATE 1000 / (float)FPS

int num_spaceships = 32;

enum {
    MOUSE_LEFT,
    MOUSE_RIGHT
};

int mouse_down[2];
int mouse_released[2];
int mouse_pressed[2];

float mx = 0.0,
      my = 0.0;

int window_w = 1500,
    window_h = 1000,
    world_w  = 1500,
    world_h  = 1500;

GLFWwindow* window;

Rect **rects = NULL;

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


/*******************************************************************************
 *
 * I/O
 *
 ******************************************************************************/

int key_down[26];

int key_pressed[26];

void set_key_down(int key, int b)
{
    if (key < 65 || key > 90) return;
    key_down[key - 65] = b;
}

void set_key_pressed(int key)
{
    if (key < 65 || key > 90) return;
    key_pressed[key - 65] = 1;
}

int is_key_down(int key)
{
    return key_down[key - 65];
}

int is_key_pressed(int key)
{
    int i = key - 65;
    int b = key_down[i];
    key_down[i] = 0;
    return b;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
    int mods)
{
    if (action == GLFW_PRESS)
    {
        set_key_down(key, 1);
        set_key_pressed(key);
    }
    if (action == GLFW_RELEASE)
    {
        set_key_down(key, 0);
    }
}

static void cursor_position_callback(GLFWwindow *window, double x, double y)
{
    mx = x;
    my = window_h - y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouse_down[MOUSE_RIGHT] = 1;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouse_pressed[MOUSE_LEFT] = 1;
    }
    if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouse_down[MOUSE_RIGHT] = 0;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouse_released[MOUSE_LEFT] = 1;
    }
}

/* Returns 1 once when 'key' is pressed, 0 otherwise */
int is_mouse_pressed(int key)
{
    int key_state = 0;
    switch (key)
    {
    case MOUSE_LEFT:
        key_state = mouse_pressed[MOUSE_LEFT];
        mouse_pressed[MOUSE_LEFT] = 0;
        return key_state;
    }

    return -1;
}

/* Returns 1 once when 'key' is released, 0 otherwise */
int is_mouse_released(int key)
{
    int key_state = 0;
    switch (key)
    {
    case MOUSE_LEFT:
        key_state = mouse_released[MOUSE_LEFT];
        mouse_released[MOUSE_LEFT] = 0;
        return key_state;
    }

    return -1;
}

/* Returns 1 as long as 'key' is down, 0 otherwise */
int is_mouse_down(int key)
{
    switch (key)
    {
    case MOUSE_RIGHT:
        return mouse_down[MOUSE_RIGHT];
    }

    return -1;
}


/*******************************************************************************
 *
 * Utils
 *
 ******************************************************************************/


void window_center(GLFWwindow *window)
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    if (!monitor)
        return;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        return;

    int monitor_x, monitor_y;
    glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    glfwSetWindowPos(
        window,
        monitor_x + (mode->width - window_width) / 2,
        monitor_y + (mode->height - window_height) / 2
    );
}

void window_initialize(void)
{
    /* Configure GLFW */

    if (!glfwInit())
    {
        printf("[ERROR] Failed to initialize glfw\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(window_w, window_h, "GLFW Window", NULL, NULL);

    if (!window)
    {
        printf("[ERROR] Failed to initialize window\n");
        exit(1);
    }

    window_center(window);
    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    /* Configure OpenGL */

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void vec2f_norm(double vec[2], double uvec[2])
{
    double size = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
    uvec[0] = vec[0] / size;
    uvec[1] = vec[1] / size;
}

double vec2f_size(double vec[2])
{
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
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

void draw_square(double x, double y)
{
    double size = world.cell_size;
    double points[8] = {
        x, y,
        x, y + size,
        x + size, y + size,
        x + size, y
    };
    glColor4f(0.0, 0.0, 0.5, 1.0);
    glLineWidth(1.0f);
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

void world_draw_region_circle(void)
{
    double cell_size = world.cell_size;
    double x = floor(p_circle->cx / cell_size) * cell_size;
    double y = floor(p_circle->cy / cell_size) * cell_size;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
            draw_square(x + dx * cell_size, y + dy * cell_size);
}

void world_draw_region_rect(void)
{
    double cell_size = world.cell_size;
    double x = p_rect->cx + cell_size / 2;
    double y = p_rect->cy + cell_size / 2;
    x = floor(x / cell_size) * cell_size;
    y = floor(y / cell_size) * cell_size;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
            draw_square(x + dx * cell_size, y + dy * cell_size);
}

void world_draw(void)
{
    for (int i = 0; i < n_rects; i++)
    {
        Rect *rect = rects[i];
        rect->draw(rect);
    }
}

void draw_circle_ray(Circle *circle, Rect *rect)
{
    double nx, ny;
    rect->minpoint(rect, circle->cx, circle->cy, &nx, &ny);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glLineWidth(1.0f);
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
    r2->minpoint(r2, x, y, &nx, &ny);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glLineWidth(1.0f);
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
    r2->minpoint(r2, cx1, cy1, &nx, &ny);

    double vec[2] = { nx - cx1, ny - cy1 };

    glColor4f(1.0, 0.0, 0.0, 1.0);
    glLineWidth(1.0f);
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
    r2->minpoint(r2, cx1, cy1, &nx, &ny);

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
    glLineWidth(1.0f);
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
    rect->minpoint(rect, circle->cx, circle->cy, &nx, &ny);

    double vec[2] = { nx - circle->cx, ny - circle->cy };
    double overlap = circle->size - vec2f_size(vec);

    if (overlap > 0.0)
    {
        double uvec[2] = { 0.0, 0.0 };
        vec2f_norm(vec, uvec);
        circle->cx -= uvec[0] * overlap;
        circle->cy -= uvec[1] * overlap;
        return 1;
    }

    return 0;
}

void world_resolve_circle_collision(void)
{
    int x, y;
    world_cell2i(p_circle->cx, p_circle->cy, &x, &y);

    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            int i = (y + dy) * world.n_cells_x + x + dx;
            if ((i < 0) || (i > world.n_cells_x * world.n_cells_y))
                continue;
            Rect *rect = world.cells[i];
            if (rect != NULL)
            {
                collide_circle_rect(p_circle, rect);
                draw_circle_ray(p_circle, rect);
            }
        }
    }
}

Rect *cell(double x, double y)
{
    int i = y * world.n_cells_x + x;
    if ((i < 0) || (i > world.n_cells_x * world.n_cells_y))
        return NULL;
    return world.cells[i];
}

void world_resolve_rect_collision(void)
{
    int x, y;
    double size = p_rect->size;
    world_cell2i(p_rect->cx + size / 2, p_rect->cy + size / 2, &x, &y);

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
    p_circle = new_circle((window_w  - size) / 2, (window_h - size) / 2, size);
}

void update_circle(void)
{
    if (is_key_down(GLFW_KEY_J))
        p_circle->turn_left(p_circle);
    if (is_key_down(GLFW_KEY_L))
        p_circle->turn_right(p_circle);
    if (is_key_down(GLFW_KEY_I))
        p_circle->move_forward(p_circle);
    if (is_key_down(GLFW_KEY_K))
        p_circle->move_backward(p_circle);
    p_circle->draw(p_circle);
}

void init_rect(void)
{
    double size = world.cell_size;
    p_rect = new_rect((window_w  - size) / 2, (window_h - size) / 2, size);
}

void update_rect(void)
{
    if (is_key_down(GLFW_KEY_A))
        p_rect->move_left(p_rect);
    if (is_key_down(GLFW_KEY_D))
        p_rect->move_right(p_rect);
    if (is_key_down(GLFW_KEY_W))
        p_rect->move_forward(p_rect);
    if (is_key_down(GLFW_KEY_S))
        p_rect->move_backward(p_rect);
    p_rect->draw(p_rect);
}

int main(int argc, char **argv)
{
    srand(42);

    memset(key_down, 0, 26 * sizeof(int));
    memset(key_pressed, 0, 26 * sizeof(int));

    window_initialize();
    world_init("worlds/world1.txt");

    init_rect();
    init_circle();

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &window_w, &window_h);
        glViewport(0, 0, window_w, window_h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, window_w, 0, window_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world_draw_region_rect();
        world_draw_region_circle();

        world_draw();

        update_rect();
        update_circle();

        world_resolve_rect_collision();
        world_resolve_circle_collision();

        if (is_key_pressed(GLFW_KEY_Q))
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

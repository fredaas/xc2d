#include "window.h"

/* Zoom */
double zoom = 1.0;
double zoom_max = 1.0 + 1.0;
double zoom_min = 1.0 - 0.75;

/* Keys */
int key_down[26];
int key_pressed[26];

/* Mouse */
double screen_mx = 0.0;
double screen_my = 0.0;
double world_mx = 0.0;
double world_my = 0.0;
int mouse_down[2] = { 0, 0 };
int mouse_released[2] = { 0, 0 };
int mouse_pressed[2] = { 0, 0 };

double window_width = 0;
double window_height = 0;

static void set_key_down(int key, int b)
{
    if (key < 65 || key > 90) return;
    key_down[key - 65] = b;
}

static void set_key_pressed(int key)
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

double window_zoom(void)
{
    return zoom;
}

void window_mouse_pos(double *x, double *y)
{
    *x = screen_mx;
    *y = screen_my;
}

static void scroll_callback(GLFWwindow *window, double dx, double dy)
{
    if (dy < 0)
    {
        zoom -= 0.1;
        if (zoom < zoom_min)
            zoom = zoom_min;
    }
    else
    {
        zoom += 0.1;
        if (zoom > zoom_max)
            zoom = zoom_max;
    }
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
    screen_mx = x;
    screen_my = window_height - y;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action,
    int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_down[MOUSE_RIGHT] = 1;
            mouse_pressed[MOUSE_RIGHT] = 1;
            break;
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_down[MOUSE_LEFT] = 1;
            mouse_pressed[MOUSE_LEFT] = 1;
            break;
        }
    }
    if (action == GLFW_RELEASE)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_down[MOUSE_RIGHT] = 0;
            break;
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_down[MOUSE_LEFT] = 0;
            break;
        }
    }
}

/* Returns 1 once when 'key' is pressed, 0 otherwise */
int is_mouse_pressed(int key)
{
    int value = 0;
    switch (key)
    {
    case MOUSE_RIGHT:
        value = mouse_pressed[MOUSE_RIGHT];
        mouse_pressed[MOUSE_RIGHT] = 0;
        return value;
    case MOUSE_LEFT:
        value = mouse_pressed[MOUSE_LEFT];
        mouse_pressed[MOUSE_LEFT] = 0;
        return value;
    }

    return -1;
}

/* Returns 1 once when 'key' is released, 0 otherwise */
static int is_mouse_released(int key)
{
    switch (key)
    {
        /* TODO: Implement this */
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
    case MOUSE_LEFT:
        return mouse_down[MOUSE_LEFT];
    }

    return -1;
}

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

Window * new_window(int width, int height)
{
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

    window_width = width;
    window_height = height;

    memset(key_down, 0, 26 * sizeof(int));
    memset(key_pressed, 0, 26 * sizeof(int));

    Window *window = (Window *)malloc(sizeof(Window));
    window->width = width;
    window->height = height;
    window->view = glfwCreateWindow(width, height, "GLFW Window",
        NULL, NULL);

    if (!window->view)
    {
        printf("[ERROR] Failed to initialize window\n");
        exit(1);
    }

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window->view);

    glfwSetKeyCallback(window->view, key_callback);
    glfwSetCursorPosCallback(window->view, cursor_position_callback);
    glfwSetMouseButtonCallback(window->view, mouse_button_callback);
    glfwSetScrollCallback(window->view, scroll_callback);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glLineWidth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return window;
}

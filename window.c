#include "window.h"

enum {
    MOUSE_LEFT,
    MOUSE_RIGHT
};

int key_down[26];
int key_pressed[26];

int mouse_down[2];
int mouse_released[2];
int mouse_pressed[2];

int window_w;
int window_h;

double zoom = 1.0;
double zoom_max = 1.0 + 1.0;
double zoom_min = 1.0 - 0.5;

float mx = 0.0;
float my = 0.0;

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
    mx = x;
    my = window_h - y;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action,
    int mods)
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
static int is_mouse_pressed(int key)
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
static int is_mouse_released(int key)
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
static int is_mouse_down(int key)
{
    switch (key)
    {
    case MOUSE_RIGHT:
        return mouse_down[MOUSE_RIGHT];
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

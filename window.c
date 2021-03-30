#include "window.h"

/* Zoom */
static double zoom = 1.0;
static double zoom_max = 1.0 + 1.0;
static double zoom_min = 1.0 - 0.75;
static double zoom_delta = 1.0;
static double zoom_target = 1.0;

/* Keys */
static int key_current[26] = { 0 };
static int key_previous[26] = { 0 };

/* Mouse */
static double screen_mx = 0.0;
static double screen_my = 0.0;
static double world_mx = 0.0;
static double world_my = 0.0;
static int mouse_current[2];
static int mouse_previous[2];

/* Window */
static double window_width = 0;
static double window_height = 0;

static void set_key(int key, int s)
{
    if (key < 65 || key > 90) return;
    key_current[key - 65] = s;
}

int is_key_down(int key)
{
    return key_current[key - 65];
}

int is_key_pressed(int key)
{
    return !key_previous[key - 65] && key_current[key - 65];
}

int is_key_released(int key)
{
    return key_previous[key - 65] && !key_current[key - 65];
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
    /* Zoom out */
    if (dy < 0)
    {
        zoom -= 0.1;
        if (zoom < zoom_min)
            zoom = zoom_min;
    }
    /* Zoom in */
    else
    {
        zoom += 0.1;
         if (zoom > zoom_max)
            zoom = zoom_max;
    }
}

/* Sets 'zoom_target' used in 'update_zoom' */
void window_zoom_target(double zoom)
{
    zoom_target = zoom;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
    int mods)
{
    if (action == GLFW_PRESS)
    {
        set_key(key, 1);
    }
    if (action == GLFW_RELEASE)
    {
        set_key(key, 0);
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
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_current[button] = 1;
            break;
        }
    }
    if (action == GLFW_RELEASE)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_RIGHT:
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_current[button] = 0;
            break;
        }
    }
}

int is_mouse_pressed(int key)
{
    return !mouse_previous[key] && mouse_current[key];
}

int is_mouse_released(int key)
{
    return mouse_previous[key] && !mouse_current[key];
}

int is_mouse_down(int key)
{
    return mouse_current[key];
}

void window_update_keys(void)
{
    memcpy(mouse_previous, mouse_current, sizeof(int) * 2);
    memcpy(key_previous, key_current, sizeof(int) * 26);
}

int window_update_zoom(void)
{
    /* Zoom out */
    if (zoom_target < zoom)
    {
        zoom -= (dt * zoom_delta);
        if (zoom < zoom_target)
        {
            zoom = zoom_target;
            return 0;
        }
    }
    /* Zoom in */
    else if (zoom_target > zoom)
    {
        zoom += (dt * zoom_delta);
        if (zoom > zoom_target)
        {
            zoom = zoom_target;
            return 0;
        }
    }

    return 1;
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

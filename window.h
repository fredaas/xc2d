#ifndef WINDOW_H
#define WINDOW_H

#include "utils.h"

enum { MOUSE_LEFT, MOUSE_RIGHT };

typedef struct Window Window;

struct Window {
    int width;
    int height;
    GLFWwindow *view;
};

Window * new_window(int width, int height);
void window_center(GLFWwindow *view);
int is_key_down(int key);
int is_key_pressed(int key);
int is_mouse_pressed(int key);
int is_mouse_down(int key);
double window_zoom(void);
void window_mouse_pos(double *x, double *y);

void update_zoom(void);
void set_zoom_target(double zoom);

#endif /* WINDOW_H */

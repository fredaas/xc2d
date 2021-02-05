#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

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
double window_zoom(void);

#endif /* WINDOW_H */

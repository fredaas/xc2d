#ifndef WORLD_H
#define WORLD_H

#include "utils.h"
#include "rect.h"
#include "draw.h"
#include "window.h"

enum { BLOCK_CIRCLE, BLOCK_RECT };

typedef struct Cell Cell;

struct Cell {
    int type;
    void *value;
    double x;
    double y;
};

typedef struct World World;

struct World {
    int n_cells_x;
    int n_cells_y;
    int cell_size;
    int n_blocks;
    Cell **blocks;
    Cell **grid;
    double x;
    double y;
    double scale;
};

World * new_world(char *path);
void world_draw_region(World *world, double x, double y, int size, int hex);
void world_draw(World *world);
void world_pos2cell(World *world, double x, double y, int *cx, int *cy);
void world_cell2pos(World *world, int i, double *x, double *y);
void world_world2screen(World *self, double x, double y, double *tx, double *ty);
void world_screen2world(World *self, double x, double y, double *tx, double *ty);
void world_update_pan(World *self);
void world_update_scale(World *self);
Cell *world_cell(World *self, int x, int y);

#endif /* WORLD_H */

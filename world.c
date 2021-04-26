#include "world.h"

/* Returns the file dimensions (rows, cols) of fp */
static void fsize(FILE *fp, int *nx, int *ny)
{
    char c;
    int cx = 0;
    int n_cells_x = 0;
    int n_cells_y = 0;
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
void world_pos2cell(World *self, double x, double y, int *cx, int *cy)
{
    double cell_size = self->cell_size;
    *cx = floor(x / cell_size);
    *cy = floor(y / cell_size);
}

/* Returns the world point (x, y) of the corresponding cell index i */
void world_cell2pos(World *self, int i, double *x, double *y)
{
    *x = (i % self->n_cells_x) * self->cell_size;
    *y = (i / self->n_cells_x) * self->cell_size;
}

World * new_world(char *path)
{
    FILE *fp = fopen(path, "r");

    World *world = (World *)malloc(sizeof(World));

    world->x = 0.0;
    world->y = 0.0;
    world->scale = 1.0;
    world->n_blocks = 0;

    fsize(fp, &world->n_cells_x, &world->n_cells_y);

    world->cell_size = 30;

    /* Create rects from file */
    Cell *blocks[256];
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
            double y = world->n_cells_y - ny - 1;
            double size = world->cell_size;
            Rect *rect = new_rect(x * size, y * size, size);
            Cell *block = (Cell *)malloc(sizeof(Cell));
            block->type = BLOCK_RECT;
            block->value = (void *)rect;
            block->x = x * size;
            block->y = y * size;
            blocks[world->n_blocks] = block;
            world->n_blocks++;
            nx++;
        }
        else if (c == 'o')
        {
            double x = nx;
            double y = world->n_cells_y - ny - 1;
            double size = world->cell_size;
            double cx = x * size + size / 2;
            double cy = y * size + size / 2;
            Circle *circle = new_circle(cx, cy, size / 2);
            Cell *block = (Cell *)malloc(sizeof(Cell));
            block->type = BLOCK_CIRCLE;
            block->value = (void *)circle;
            block->x = cx;
            block->y = cy;
            blocks[world->n_blocks] = block;
            world->n_blocks++;
            nx++;
        }
    }

    /* Initialize world blocks */
    world->blocks = (Cell **)malloc(
        world->n_blocks * sizeof(Cell *)
    );
    for (int i = 0; i < world->n_blocks; i++)
        world->blocks[i] = blocks[i];

    /* Initialize world grid */
    world->grid = (Cell **)malloc(
        world->n_cells_x * world->n_cells_y * sizeof(Cell *)
    );
    for (int i = 0; i < world->n_cells_x * world->n_cells_y; i++)
        world->grid[i] = NULL;

    /* Add blocks to world grid */
    for (int i = 0; i < world->n_blocks; i++)
    {
        Cell *block = world->blocks[i];
        int x, y;
        world_pos2cell(world, block->x, block->y, &x, &y);
        world->grid[y * world->n_cells_x + x] = block;
    }

    return world;
}

void world_draw_region(World *self, double x, double y, int size, int hex)
{
    int cell_size = self->cell_size;
    int region_size = cell_size * size;

    /* Translate (x, y) to center cell point */
    x = floor(x / cell_size) * cell_size + cell_size / 2.0;
    y = floor(y / cell_size) * cell_size + cell_size / 2.0;

    /* Set region local origin */
    double cx = x - region_size / 2;
    double cy = y - region_size / 2;

    /* Draw grid */
    draw_square(cx, cy, region_size, hex);
    glBegin(GL_LINES);
    for (int i = 1; i <= (size - 1); i++)
    {
        glVertex2f(cx + i * cell_size, y - region_size / 2);
        glVertex2f(cx + i * cell_size, y + region_size / 2);
    }
    for (int i = 1; i <= (size - 1); i++)
    {
        glVertex2f(x - region_size / 2, cy + i * cell_size);
        glVertex2f(x + region_size / 2, cy + i * cell_size);
    }
    glEnd();
}

void world_draw(World *self)
{
    for (int i = 0; i < self->n_blocks; i++)
    {
        Cell *block = self->blocks[i];
        if (block->type == BLOCK_RECT)
        {
            Rect *rect = (Rect *)block->value;
            draw_square(rect->cx, rect->cy, rect->size, 0xffffffff);
        }
        else if (block->type == BLOCK_CIRCLE)
        {
            Circle *circle = (Circle *)block->value;
            draw_circle(circle->cx, circle->cy, circle->size, 0xffffffff);
        }
    }
}

void world_world2screen(World *self, double x, double y, double *tx, double *ty)
{
    *tx = (x + self->x) * self->scale;
    *ty = (y + self->y) * self->scale;
}

void world_screen2world(World *self, double x, double y, double *tx, double *ty)
{
    *tx = (x / self->scale) - self->x;
    *ty = (y / self->scale) - self->y;
}

void world_update_pan(World *self)
{
    static double px, py;

    if (is_mouse_pressed(MOUSE_LEFT))
    {
        window_mouse_pos(&px, &py);
    }

    if (is_mouse_down(MOUSE_LEFT))
    {
        double mx, my;
        window_mouse_pos(&mx, &my);
        self->x += (mx - px) / self->scale;
        self->y += (my - py) / self->scale;
        px = mx;
        py = my;
    }
}

void world_update_scale(World *self)
{
    double x, y, wx0, wy0, wx1, wy1;
    window_mouse_pos(&x, &y);
    world_screen2world(self, x, y, &wx0, &wy0);
    self->scale = window_zoom();
    glScaled(self->scale, self->scale, 1);
    world_screen2world(self, x, y, &wx1, &wy1);
    self->x -= (wx0 - wx1);
    self->y -= (wy0 - wy1);
}

Cell *world_cell(World *self, int x, int y)
{
    if ((x < 0) || (x >= self->n_cells_x) || (y < 0) || (y >= self->n_cells_y))
        return NULL;
    return self->grid[y * self->n_cells_x + x];
}

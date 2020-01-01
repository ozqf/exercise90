#ifndef ZPG_DRAW_GRID_PRIMITIVES_H
#define ZPG_DRAW_GRID_PRIMITIVES_H

#include "ze_proc_gen.h"

static void ZPG_DrawLine(ZPGGrid *grid, f32 x0, f32 y0, f32 x1, f32 y1, i32 typeToPaint)
{
    float dx = x1 - x0;
    if (dx < 0)
    {
        dx = -dx;
    }
    float dy = y1 - y0;
    if (dy < 0)
    {
        dy = -dy;
    }

    i32 plotX = (i32)x0;
    i32 plotY = (i32)y0;

    i32 n = 1;
    i32 x_inc;
    i32 y_inc;
    float error;

    if (dx == 0)
    {
        x_inc = 0;
        error = INFINITY;
    }
    else if (x1 > x0)
    {
        x_inc = 1;
        n += (int)x1 - plotX;
        error = ((int)x0 + 1 - x0) * dy;
    }
    else
    {
        x_inc = -1;
        n += plotX - (int)x1;
        error = (x0 - (int)x0) * dy;
    }

    if (dy == 0)
    {
        y_inc = 0;

        error = -INFINITY;
    }
    else if (y1 > y0)
    {
        y_inc = 1;
        n += (int)y1 - plotY;
        error -= ((int)y0 + 1 - y0) * dx;
    }
    else
    {
        y_inc = -1;
        n += plotY - (int)y1;
        error -= (y0 - (int)y0) * dx;
    }

    for (; n > 0; --n)
    {
        ZPGCell* cell = grid->GetCellAt(plotX, plotY);
        if (cell != NULL)
        {
            cell->type = typeToPaint;
        }

        if (error > 0)
        {
            plotY += y_inc;
            error -= dx;
        }
        else
        {
            plotX += x_inc;
            error += dy;
        }
    }
}

#endif // ZPG_DRAW_GRID_PRIMITIVES_H
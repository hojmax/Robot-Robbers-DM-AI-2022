#include "constants.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int* get_move(int sx, int sy, int ex, int ey, int* came_from)
{
    int index;
    do {
        index = ey * W + ex;
        int previous = came_from[index];
        ex = previous % W;
        ey = previous / W;
    } while (sx != ex || sy != ey);
    int second_to_last_x = index % W;
    int second_to_last_y = index / W;
    int* move = calloc(2, sizeof(int));
    move[0] = second_to_last_x - sx;
    move[1] = second_to_last_y - sy;
    return move;
}

int* get_a_star_move(int sx, int sy, int tx, int ty, int* obstacle_map,
    int* (*get_neighbours)(int, int, int*, int*), int (*heuristic)(int, int, int, int, int*),
    int (*is_done)(int, int, int, int, int*), int* cashbag_map)
{
    int* came_from = calloc(W * H, sizeof(int));
    int* cost_so_far = calloc(W * H, sizeof(int));

    int n = 0;
    int* keys = calloc(MAX_HEAP_SIZE, sizeof(int));
    int* values = calloc(MAX_HEAP_SIZE, sizeof(int));

    int start_index = sy * W + sx;
    insert(keys, values, 0, start_index, &n);
    cost_so_far[start_index] = 1;
    came_from[start_index] = start_index;
    int cx;
    int cy;
    while (n > 0) {
        int current = extract_max(keys, values, &n);
        cx = current % W;
        cy = current / W;
        if (is_done(cx, cy, tx, ty, obstacle_map)) {
            break;
        }
        int* neighbours = get_neighbours(cx, cy, obstacle_map, cashbag_map);
        for (int i = 0; i < N_NEIGHBOURS; i++) {
            int nx = neighbours[2 * i];
            int ny = neighbours[2 * i + 1];
            if (nx == -1 || ny == -1)
                continue;
            int neighbour_index = ny * W + nx;
            int new_cost = cost_so_far[cy * W + cx] + 1;
            int current_cost = cost_so_far[neighbour_index];
            int is_new = current_cost == 0;
            int is_better = new_cost < current_cost;
            if (is_new || is_better) {
                cost_so_far[neighbour_index] = new_cost;
                int priority = new_cost + heuristic(nx, ny, tx, ty, obstacle_map);
                insert(keys, values, -priority, neighbour_index, &n);
                came_from[neighbour_index] = current;
            }
        }
        free(neighbours);
    }
    free(keys);
    free(values);
    int isEmpty = n == 0;
    if (isEmpty) {
        return NULL;
    }
    return get_move(sx, sy, cx, cy, came_from);
}
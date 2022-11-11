#include "ai.h"
#include "binary_heap.h"
#include "constants.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int* get_initial_move(int sx, int sy, int ex, int ey, int* came_from)
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

int* get_neighbours_base(
    int x, int y, int (*neighbour_condition)(int, int, StateInfo), StateInfo info)
{
    int* output = calloc(2 * N_NEIGHBOURS, sizeof(int));
    for (int i = 0; i < 2 * N_NEIGHBOURS; i++) {
        output[i] = -1;
    }
    int index = 0;
    int all_moves[] = { 1, 0, -1, 0, 0, 1, 0, -1, 1, 1, 1, -1, -1, 1, -1, -1 };
    for (int i = 0; i < N_NEIGHBOURS; i++) {
        int dx = all_moves[2 * i];
        int dy = all_moves[2 * i + 1];
        int nx = x + dx;
        int ny = y + dy;
        int is_bounded = nx >= 0 && nx < W && ny >= 0 && ny < H;
        if (is_bounded && neighbour_condition(nx, ny, info)) {
            int index2 = ny * W + nx;
            output[2 * index] = nx;
            output[2 * index + 1] = ny;
            index++;
        }
    }
    return output;
}

int get_custom_distance(int x1, int y1, int x2, int y2)
{
    int delta_x = abs(x1 - x2);
    int delta_y = abs(y1 - y2);
    int distance = delta_x + delta_y - min(delta_x, delta_y);
    return distance;
}

int herding_heuristic(int sx, int sy, int ex, int ey, StateInfo info)
{
    return get_custom_distance(sx, sy, ex, ey);
}

int herding_is_done(int sx, int sy, int ex, int ey, StateInfo info)
{
    int distance = get_custom_distance(sx, sy, ex, ey);
    return distance < SCROOGE_RADIUS_CUSTOM;
}

int herding_neighbour_condition(int x, int y, StateInfo info)
{
    int index = y * W + x;
    int is_cashbag = info.cashbag_map[index];
    int is_obstacle = info.padded_obstacle_map[index];
    int is_scrooge = info.scrooge_map[index];
    return !(is_cashbag || is_obstacle || is_scrooge);
}

int* get_herding_neighbours(int x, int y, StateInfo info)
{
    return get_neighbours_base(x, y, herding_neighbour_condition, info);
}

int flee_neighbour_condition(int x, int y, StateInfo info)
{
    int index = y * W + x;
    int is_cashbag = info.cashbag_map[index];
    int is_obstacle = info.obstacle_map[index];
    int is_scrooge = info.scrooge_map[index];
    return !(is_cashbag || is_obstacle || is_scrooge);
}

int flee_heuristic(int sx, int sy, int ex, int ey, StateInfo info) { return 0; }

int flee_is_done(int sx, int sy, int ex, int ey, StateInfo info)
{
    int index = sy * W + sx;
    return !info.obstacle_map[index] && !info.scrooge_radius_map[index];
}

int* get_flee_neighbours(int x, int y, StateInfo info)
{
    return get_neighbours_base(x, y, flee_neighbour_condition, info);
}

int* get_a_star_move(int sx, int sy, int tx, int ty, int* (*get_neighbours)(int, int, StateInfo),
    int (*heuristic)(int, int, int, int, StateInfo), int (*is_done)(int, int, int, int, StateInfo),
    StateInfo info)
{
    int* came_from = calloc(W * H, sizeof(int));
    int* cost_so_far = calloc(W * H, sizeof(int));

    int n = 0;
    int* keys = calloc(W * H, sizeof(int));
    int* values = calloc(W * H, sizeof(int));

    int start_index = sy * W + sx;
    insert(keys, values, 0, start_index, &n);
    cost_so_far[start_index] = 1;
    came_from[start_index] = start_index;
    int cx;
    int cy;
    while (n > 0) {
        // printf("n: %d\n", n);
        int current = extract_max(keys, values, &n);
        cx = current % W;
        cy = current / W;
        if (is_done(cx, cy, tx, ty, info)) {
            break;
        }
        int* neighbours = get_neighbours(cx, cy, info);
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
                int priority = new_cost + heuristic(nx, ny, tx, ty, info);
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
    return get_initial_move(sx, sy, cx, cy, came_from);
}

int* flee_a_star(int x, int y, StateInfo info)
{
    return get_a_star_move(x, y, 0, 0, get_flee_neighbours, flee_heuristic, flee_is_done, info);
}

int* herding_a_star(int sx, int sy, int ex, int ey, StateInfo info)
{
    return get_a_star_move(
        sx, sy, ex, ey, get_herding_neighbours, herding_heuristic, herding_is_done, info);
}
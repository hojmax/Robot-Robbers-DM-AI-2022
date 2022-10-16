#include "ai.h"
#include "a_star.h"
#include "constants.h"
#include "map.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char end_color[] = "\033[0m";
const char colors[8][11] = {
    "\033[0;30m",
    "\033[0;36m",
    "\033[0;37m",
    "\033[0;34m",
    "\033[0;33m",
    "\033[0;32m",
    "\033[0;35m",
    "\033[0;31m",
};

void print_grid(int* grid, int w, int h)
{
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int value = grid[i * w + j];
            printf("%s%d%s ", colors[value], value, end_color);
        }
        printf("\n");
    }
    printf("\n");
}

StateInfo get_state_info(int* scrooges, int* cashbags, int* dropspots, int* obstacles,
    int n_scrooges, int n_cashbags, int n_dropspots, int n_obstacles)
{
    int* obstacle_map = get_obstacle_map(obstacles, n_obstacles);
    int* cashbag_map = get_cashbag_map(cashbags, n_cashbags);
    int* scrooge_map = get_scrooge_map(scrooges, n_scrooges);
    int* scrooge_radius_map = get_scrooge_radius_map(scrooges, n_scrooges);
    int* dropspot_map = get_dropspot_map(dropspots, n_dropspots);
    return (StateInfo) { .obstacle_map = obstacle_map,
        .cashbag_map = cashbag_map,
        .scrooge_map = scrooge_map,
        .scrooge_radius_map = scrooge_radius_map,
        .dropspot_map = dropspot_map,
        .misc_value = 0 };
}

int* get_action(int* robots, int* scrooges, int* cashbags, int* dropspots, int* cash_carried,
    int* obstacles, int n_robots, int n_scrooges, int n_cashbags, int n_dropspots,
    int n_cash_carried, int n_obstacles, int game_ticks)
{
    int* actions = calloc(2 * n_robots, sizeof(int));
    StateInfo info = get_state_info(
        scrooges, cashbags, dropspots, obstacles, n_scrooges, n_cashbags, n_dropspots, n_obstacles);
    for (int i = 0; i < n_robots; i++) {
        int rx = robots[2 * i];
        int ry = robots[2 * i + 1];
        if (i == HERDING_ROBOT) {

        } else if (i == GREED_ROBOT) {

        } else {
            int is_watched = info.scrooge_radius_map[ry * W + rx];
            if (is_watched) {
                int* move = flee_a_star(rx, ry, info);
                if (move != NULL) {
                    actions[2 * i] = move[0];
                    actions[2 * i + 1] = move[1];
                }
            }
        }
    }
    free(info.obstacle_map);
    free(info.cashbag_map);
    free(info.scrooge_map);
    free(info.scrooge_radius_map);
    free(info.dropspot_map);
    return actions;
}

// -- MAX HEAP TEST
// int n = 0;
// int* keys = calloc(MAX_HEAP_SIZE, sizeof(int));
// int* values = calloc(MAX_HEAP_SIZE, sizeof(int));
// insert(keys, values, 10, 45, &n);
// insert(keys, values, 92, 15, &n);
// insert(keys, values, 53, 75, &n);
// insert(keys, values, 12, 40, &n);
// printf("%d\n", extract_max(keys, values, &n));
// printf("%d\n", extract_max(keys, values, &n));
// printf("%d\n", extract_max(keys, values, &n));
// printf("%d\n\n", extract_max(keys, values, &n));

// cd ai && make ai && cd .. && python main.py
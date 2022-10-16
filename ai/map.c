#include "constants.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int* get_radius_map()
{
    int side_length = SCROOGE_RADIUS * 2 + 1;
    int* radius_map = calloc(side_length * side_length, sizeof(int));
    for (int i = 0; i < side_length; i++) {
        for (int j = 0; j < side_length; j++) {
            int x = i - SCROOGE_RADIUS;
            int y = j - SCROOGE_RADIUS;
            float dist = sqrt(x * x + y * y);
            if (dist <= SCROOGE_RADIUS) {
                radius_map[i * side_length + j] = 1;
            }
        }
    }
    return radius_map;
}

int* get_scrooge_radius_map(int* scrooges, int n_scrooges)
{
    int* scrooge_map = calloc(W * H, sizeof(int));
    int* radius_map = get_radius_map();

    for (int i = 0; i < n_scrooges; i++) {
        int sx = scrooges[2 * i];
        int sy = scrooges[2 * i + 1];
        int w = SCROOGE_RADIUS * 2 + 1;
        int h = SCROOGE_RADIUS * 2 + 1;
        for (int j = 0; j < h; j++) {
            for (int k = 0; k < w; k++) {
                int value = radius_map[j * w + k];
                int map_x = sx + k - SCROOGE_RADIUS;
                int map_y = sy + j - SCROOGE_RADIUS;
                if (map_x >= 0 && map_x < W && map_y >= 0 && map_y < H) {
                    int index = map_y * W + map_x;
                    scrooge_map[index] += value;
                }
            }
        }
    }

    free(radius_map);
    return scrooge_map;
}

int* get_obstacle_map(int* obstacles, int n_obstacles)
{
    int* obstacle_map = calloc(W * H, sizeof(int));

    for (int i = 0; i < n_obstacles; i++) {
        int x = obstacles[4 * i];
        int y = obstacles[4 * i + 1];
        int w = obstacles[4 * i + 2];
        int h = obstacles[4 * i + 3];
        // Bug in their code, means j <= h and not j < h. Same for k.
        for (int j = 0; j <= h; j++) {
            int new_y = y + j;
            if (new_y >= H) {
                break;
            }
            for (int k = 0; k <= w; k++) {
                int new_x = x + k;
                if (new_x >= W) {
                    break;
                }
                obstacle_map[new_y * W + new_x] = 1;
            }
        }
    }

    return obstacle_map;
}

int* get_entity_map(int* entities, int n_entities, int padding)
{
    int* entity_map = calloc(W * H, sizeof(int));

    for (int i = 0; i < n_entities; i++) {
        int x = entities[2 * i];
        int y = entities[2 * i + 1];
        for (int j = -padding; j <= padding; j++) {
            for (int k = -padding; k <= padding; k++) {
                int new_x = x + k;
                int new_y = y + j;
                if (new_x >= 0 && new_x < W && new_y >= 0 && new_y < H) {
                    entity_map[new_y * W + new_x] = 1;
                }
            }
        }
    }

    return entity_map;
}

int* get_scrooge_map(int* scrooges, int n_scrooges)
{
    return get_entity_map(scrooges, n_scrooges, 1);
}

int* get_cashbag_map(int* cashbags, int n_cashbags)
{
    return get_entity_map(cashbags, n_cashbags, 0);
}

int* get_dropspot_map(int* dropspots, int n_dropspots)
{
    return get_entity_map(dropspots, n_dropspots, 0);
}
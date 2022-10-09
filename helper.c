#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int OBSTACLE_INDEX = 9;
int SCROOGE_INDEX = 8;
int SCROOGE_RADIUS = 15;
int W = 128;
int H = 128;

void print_grid(int* grid, int w, int h)
{
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int value = grid[i * w + j];
            printf("%d ", value);
        }
        printf("\n");
    }
    printf("\n");
}

int* get_radius_map(int radius)
{
    int side_length = radius * 2 + 1;
    int* output = calloc(side_length * side_length, sizeof(int));
    for (int i = 0; i < radius * 2 + 1; i++) {
        for (int j = 0; j < radius * 2 + 1; j++) {
            int x = i - radius;
            int y = j - radius;
            float dist = sqrt(x * x + y * y);
            if (dist <= SCROOGE_RADIUS) {
                output[i * side_length + j] = SCROOGE_INDEX;
            } else if (dist <= radius) {
                output[i * side_length + j] = radius - (int)(dist) + 1;
            }
        }
    }
    return output;
}

int* get_obstacle_map(int* scrooges, int* obstacles, int n_scrooges, int n_obstacles)
{
    int* output = calloc(W * H, sizeof(int));
    int padded_radius = SCROOGE_RADIUS + 2;
    int* radius_map = get_radius_map(padded_radius);
    for (int i = 0; i < n_scrooges; i += 2) {
        int x = scrooges[i];
        int y = scrooges[i + 1];
        int w = padded_radius * 2 + 1;
        int h = padded_radius * 2 + 1;
        for (int j = 0; j < h; j++) {
            for (int k = 0; k < w; k++) {
                int value = radius_map[j * w + k];
                int x2 = x + k - padded_radius;
                int y2 = y + j - padded_radius;
                if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
                    int index = y2 * W + x2;
                    if (output[index] < value) {
                        output[index] = value;
                    }
                }
            }
        }
    }
    for (int i = 0; i < n_obstacles; i += 4) {
        int x = obstacles[i];
        int y = obstacles[i + 1];
        int w = obstacles[i + 2];
        int h = obstacles[i + 3];
        // Bug in their code, means j <= h and not j < h
        for (int j = 0; j <= h; j++) {
            for (int k = 0; k <= w; k++) {
                output[(y + j) * W + x + k] = OBSTACLE_INDEX;
            }
        }
    }
    return output;
}

void get_action(int* robots, int* scrooges, int* cashbags, int* dropspots, int* cash_carried,
    int* obstacles, int n_robots, int n_scrooges, int n_cashbags, int n_dropspots,
    int n_cash_carried, int n_obstacles)
{
    int* obstacle_map = get_obstacle_map(scrooges, obstacles, n_scrooges, n_obstacles);
    print_grid(obstacle_map, W, H);
}

// Command: make helper && python main.py
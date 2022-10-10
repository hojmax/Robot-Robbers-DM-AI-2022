#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
int ALL_MOVES[] = { 0, 0, 0, 1, 0, -1, 1, 0, -1, 0, 1, 1, 1, -1, -1, 1, -1, -1 };
int OBSTACLE_INDEX = 9;
int SCROOGE_INDEX = 8;
int SCROOGE_RADIUS = 15;
int ACTION_SIZE = 10;
int PLAYER_ROBOTS = 5;
int N_NEIGHBOURS = 8;
int W = 128;
int H = 128;
#define MAX_QUEUE_SIZE (128 * 128)

int isEmpty(int idx) { return idx == -1; }

int isFull(int idx) { return idx == MAX_QUEUE_SIZE - 1; }

// enqueue just adds item to the end of the priority queue | O(1)
void enqueue(int data, int priority, int* idx, int* pqVal, int* pqPriority)
{
    if (!isFull(*idx)) {
        // Increase the index
        *idx += 1;

        // Insert the element in priority queue
        pqVal[*idx] = data;
        pqPriority[*idx] = priority;
    }
}

// returns item with highest priority
// NOTE: Max Priority Queue High priority number means higher priority | O(N)
int peek(int idx, int* pqVal, int* pqPriority)
{
    // Note : Max Priority, so assigned min value as initial value
    int maxPriority = INT_MIN;
    int indexPos = -1;

    // Linear search for highest priority
    for (int i = 0; i <= idx; i++) {
        // If two items have same priority choose the one with
        // higher data value
        if (maxPriority == pqPriority[i] && indexPos > -1 && pqVal[indexPos] < pqVal[i]) {
            maxPriority = pqPriority[i];
            indexPos = i;
        }

        // note: using MAX Priority so higher priority number
        // means higher priority
        else if (maxPriority < pqPriority[i]) {
            maxPriority = pqPriority[i];
            indexPos = i;
        }
    }

    // Return index of the element where
    return pqVal[indexPos];
}

// This removes the element with highest priority
// from the priority queue | O(N)
int dequeue(int* idx, int* pqVal, int* pqPriority)
{
    if (!isEmpty(*idx)) {
        // Get element with highest priority
        int indexPos = peek(*idx, pqVal, pqPriority);

        // reduce size of priority queue by first
        // shifting all elements one position left
        // from index where the highest priority item was found
        for (int i = indexPos; i < *idx; i++) {
            pqVal[i] = pqVal[i + 1];
            pqPriority[i] = pqPriority[i + 1];
        }

        // reduce size of priority queue by 1
        *idx -= 1;
        return indexPos;
    }
    return -1;
}

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

int get_nearest_dropspot(int rx, int ry, int* dropspots, int n_dropspots, int* obstacle_map)
{
    int min_distance = INT_MAX;
    int min_index = -1;
    for (int i = 0; i < n_dropspots; i += 2) {
        int dx = dropspots[i];
        int dy = dropspots[i + 1];
        int is_free = obstacle_map[dy, dx] != SCROOGE_INDEX;
        if (!is_free)
            continue;
        int delta_x = abs(rx - dx);
        int delta_y = abs(ry - dy);
        int distance = delta_x + delta_y - min(delta_x, delta_y);
        if (distance < min_distance) {
            min_distance = distance;
            min_index = i / 2;
        }
    }
    return min_index;
}

int* a_star(int sx, int sy, int tx, int ty, int* obstacle_map,
    int* (*get_neighbours)(int, int, int*), int (*heuristic)(int, int, int, int),
    int (*is_done)(int, int, int, int, int*))
{
    int* came_from = calloc(W * H, sizeof(int));
    int* cost_so_far = calloc(W * H, sizeof(int));
    // -- Priority queue of nodes to visit
    int idx = -1;
    int pqVal[MAX_QUEUE_SIZE];
    int pqPriority[MAX_QUEUE_SIZE];
    // --
}

void display(int idx, int* pqVal, int* pqPriority)
{
    for (int i = 0; i <= idx; i++) {
        printf("(%d, %d)\n", pqVal[i], pqPriority[i]);
    }
}

int* get_action(int* robots, int* scrooges, int* cashbags, int* dropspots, int* cash_carried,
    int* obstacles, int n_robots, int n_scrooges, int n_cashbags, int n_dropspots,
    int n_cash_carried, int n_obstacles)
{
    int* action = calloc(ACTION_SIZE, sizeof(int));
    int* obstacle_map = get_obstacle_map(scrooges, obstacles, n_scrooges, n_obstacles);
    print_grid(obstacle_map, W, H);

    // int* free_robots = calloc(PLAYER_ROBOTS, sizeof(int));
    // for (int i = 0; i < n_robots; i += 2) {
    //     int x = robots[i];
    //     int y = robots[i + 1];
    //     int is_free = obstacle_map[y * W + x] != SCROOGE_INDEX;
    //     int is_holding_cash = cash_carried[i / 2];
    //     if (is_free && is_holding_cash) {

    //     } else if (is_free) {
    //         free_robots[i / 2] = 1;
    //     } else {
    //     }
    // }
    int idx = -1;
    int* pqVal = calloc(MAX_QUEUE_SIZE, sizeof(int));
    int* pqPriority = calloc(MAX_QUEUE_SIZE, sizeof(int));
    enqueue(5, 1, &idx, pqVal, pqPriority);
    enqueue(10, 3, &idx, pqVal, pqPriority);
    enqueue(15, 4, &idx, pqVal, pqPriority);
    enqueue(20, 5, &idx, pqVal, pqPriority);
    enqueue(500, 2, &idx, pqVal, pqPriority);

    printf("Before Dequeue : \n");
    display(idx, pqVal, pqPriority);

    // Dequeue the top element
    printf("%d", (dequeue(&idx, pqVal, pqPriority))); // 20 dequeued
    dequeue(&idx, pqVal, pqPriority); // 15 dequeued

    printf("\nAfter Dequeue : \n");
    display(idx, pqVal, pqPriority);

    return action;
}

// Command: make helper && python main.py
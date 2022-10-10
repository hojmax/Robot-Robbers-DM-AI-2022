#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
int OBSTACLE_INDEX = 9;
int SCROOGE_INDEX = 8;
int SCROOGE_RADIUS = 15;
int ACTION_SIZE = 10;
int PLAYER_ROBOTS = 5;
int N_NEIGHBOURS = 16;
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
        if (maxPriority < pqPriority[i]) {
            maxPriority = pqPriority[i];
            indexPos = i;
        }
    }

    // Return index of the element where
    return indexPos;
}

// This removes the element with highest priority
// from the priority queue | O(N)
int dequeue(int* idx, int* pqVal, int* pqPriority)
{
    if (!isEmpty(*idx)) {
        // Get element with highest priority
        int indexPos = peek(*idx, pqVal, pqPriority);
        int returnValue = pqVal[indexPos];
        // reduce size of priority queue by first
        // shifting all elements one position left
        // from index where the highest priority item was found
        for (int i = indexPos; i < *idx; i++) {
            pqVal[i] = pqVal[i + 1];
            pqPriority[i] = pqPriority[i + 1];
        }

        // reduce size of priority queue by 1
        *idx -= 1;
        return returnValue;
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
                output[min((y + j), H - 1) * W + min(x + k, W - 1)] = OBSTACLE_INDEX;
            }
        }
    }
    free(radius_map);
    return output;
}

int get_custom_distance(int x1, int y1, int x2, int y2)
{
    int delta_x = abs(x1 - x2);
    int delta_y = abs(y1 - y2);
    int distance = delta_x + delta_y - min(delta_x, delta_y);
    return distance;
}

int get_nearest_dropspot(int rx, int ry, int* dropspots, int n_dropspots, int* obstacle_map)
{
    int min_distance = INT_MAX;
    int min_index = -1;
    for (int i = 0; i < n_dropspots; i += 2) {
        int dx = dropspots[i];
        int dy = dropspots[i + 1];
        printf("Checking dropspot %d %d\n", dx, dy);
        int is_free = obstacle_map[dy * W + dx] != SCROOGE_INDEX;
        if (!is_free)
            continue;
        int distance = get_custom_distance(rx, ry, dx, dy);
        if (distance < min_distance) {
            min_distance = distance;
            min_index = i / 2;
        }
    }
    return min_index;
}

int* get_move(int sx, int sy, int ex, int ey, int* came_from)
{
    int index;
    do {
        index = ey * W + ex;
        int previous = came_from[index];
        ex = previous % W;
        ey = previous / W;
    } while (sx != ex || sy != ey);
    int second_x = index % W;
    int second_y = index / W;
    int* output = calloc(2, sizeof(int));
    output[0] = second_x - sx;
    output[1] = second_y - sy;
    return output;
}

int manhatten_repulsion_heuristic(int sx, int sy, int ex, int ey, int* obstacle_map)
{
    // Adding obstacle_map for encouarging the path to go around scrooges
    return get_custom_distance(sx, sy, ex, ey) + obstacle_map[sy * W + sx];
}

int* get_neighbours_base(int x, int y, int* obstacle_map, int (*condition)(int))
{
    int* output = calloc(16, sizeof(int));
    // needs to be -1 from beginning
    for (int i = 0; i < 16; i++) {
        output[i] = -1;
    }
    int index = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;
            int x2 = x + i;
            int y2 = y + j;
            if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
                int index2 = y2 * W + x2;
                int value = obstacle_map[index2];
                if (condition(value)) {
                    output[index] = x2;
                    output[index + 1] = y2;
                    index += 2;
                }
            }
        }
    }
    return output;
}

int* get_flee_neighbours(int x, int y, int* obstacle_map)
{
    int flee_condition(int value) { return value != OBSTACLE_INDEX; }
    return get_neighbours_base(x, y, obstacle_map, flee_condition);
}

int* get_sneaky_neighbours(int x, int y, int* obstacle_map)
{
    int sneaky_condition(int value) { return value != OBSTACLE_INDEX && value != SCROOGE_INDEX; }
    return get_neighbours_base(x, y, obstacle_map, sneaky_condition);
}

int has_reached_goal(int sx, int sy, int ex, int ey, int* obstacle_map)
{
    return sx == ex && sy == ey;
}

void display(int idx, int* pqVal, int* pqPriority)
{
    for (int i = 0; i <= idx; i++) {
        int x = pqVal[i] % W;
        int y = pqVal[i] / W;
        printf("(%d, %d, %d)\n", x, y, pqPriority[i]);
    }
    if (idx == -1) {
        printf("Empty\n");
    }
    printf("\n");
}

int* get_a_star_move(int sx, int sy, int tx, int ty, int* obstacle_map,
    int* (*get_neighbours)(int, int, int*), int (*heuristic)(int, int, int, int, int*),
    int (*is_done)(int, int, int, int, int*))
{
    int* came_from = calloc(W * H, sizeof(int));
    int* cost_so_far = calloc(W * H, sizeof(int));
    // -- Priority queue of cells to visit
    int idx = -1;
    int pqVal[MAX_QUEUE_SIZE];
    int pqPriority[MAX_QUEUE_SIZE];
    // --
    int start_index = sy * W + sx;
    enqueue(start_index, -1, &idx, pqVal, pqPriority);
    cost_so_far[start_index] = 1;
    came_from[start_index] = start_index;
    // display(idx, pqVal, pqPriority);
    int cx;
    int cy;
    while (!isEmpty(idx)) {
        // printf("Before");
        // display(idx, pqVal, pqPriority);
        int current = dequeue(&idx, pqVal, pqPriority);
        // printf("Dequeued: %d, %d\n", current % W, current / W);

        // printf("After");
        // display(idx, pqVal, pqPriority);
        cx = current % W;
        cy = current / W;
        // printf("CX: %d, CY: %d\n", cx, cy);
        if (is_done(cx, cy, tx, ty, obstacle_map)) {
            break;
        }
        int* neighbours = get_neighbours(cx, cy, obstacle_map);
        // print_grid(neighbours, N_NEIGHBOURS, 1);
        for (int i = 0; i < N_NEIGHBOURS; i += 2) {
            int nx = neighbours[i];
            int ny = neighbours[i + 1];
            // printf("NX: %d, NY: %d\n", nx, ny);
            if (nx == -1 || ny == -1)
                continue;
            int new_cost = cost_so_far[cy * W + cx] + 1;
            int neighbour_index = ny * W + nx;
            int is_new = cost_so_far[neighbour_index] == 0;
            int is_better = new_cost < cost_so_far[neighbour_index];
            if (is_new || is_better) {
                cost_so_far[neighbour_index] = new_cost;
                int priority = new_cost + heuristic(nx, ny, tx, ty, obstacle_map);
                // Minus sign because of max priority queue
                // printf("neighbour_index: %d\n", neighbour_index);
                enqueue(neighbour_index, -priority, &idx, pqVal, pqPriority);
                came_from[neighbour_index] = current;
            }
        }
        free(neighbours);
        // printf("Added neighbours\n");
        // display(idx, pqVal, pqPriority);
    }
    // print_grid(came_from, W, H);
    if (isEmpty(idx)) {
        // printf("No path found\n");
        return NULL;
    }
    // printf("A* found a path\n");
    return get_move(sx, sy, cx, cy, came_from);
}

int* get_distance_matrix(
    int* robots, int* free_robots, int* cashbags, int n_cashbags, int n_robots, int* obstacle_map)
{
    int* distance_matrix = calloc(PLAYER_ROBOTS * (n_cashbags / 2), sizeof(int));
    // Default entire distance_matrix to INT_MAX
    for (int i = 0; i < PLAYER_ROBOTS * (n_cashbags / 2); i++) {
        distance_matrix[i] = INT_MAX;
    }
    for (int i = 0; i < n_robots; i += 2) {
        int rx = robots[i];
        int ry = robots[i + 1];
        // printf("Rx %d, Ry %d\n", rx, ry);
        for (int j = 0; j < n_cashbags; j += 2) {
            int cx = cashbags[j];
            int cy = cashbags[j + 1];
            // printf("Cx %d, Cy %d\n", cx, cy);
            int distance;
            if (!free_robots[i / 2] || obstacle_map[cy * W + cx] == SCROOGE_INDEX) {
                distance = INT_MAX;
            } else {
                distance = get_custom_distance(rx, ry, cx, cy);
            }
            // printf("Distance from (%d, %d) to (%d, %d) is %d\n", rx, ry, cx, cy, distance);
            distance_matrix[(j / 2) * (n_robots / 2) + (i / 2)] = distance;
        }
    }
    return distance_matrix;
}

int null_heuristic(int sx, int sy, int ex, int ey, int* obstacle_map) { return 0; }

int not_watched_by_scrooge(int sx, int sy, int ex, int ey, int* obstacle_map)
{
    return obstacle_map[sy * W + sx] != SCROOGE_INDEX;
}

int* get_action(int* robots, int* scrooges, int* cashbags, int* dropspots, int* cash_carried,
    int* obstacles, int n_robots, int n_scrooges, int n_cashbags, int n_dropspots,
    int n_cash_carried, int n_obstacles)
{
    int* action = calloc(ACTION_SIZE, sizeof(int));
    int* obstacle_map = get_obstacle_map(scrooges, obstacles, n_scrooges, n_obstacles);
    print_grid(obstacle_map, W, H);
    int* obstacle_map_2 = calloc(W * H, sizeof(int));
    for (int i = 0; i < W * H; i++) {
        obstacle_map_2[i] = obstacle_map[i];
    }

    int n_free_robots = 0;
    int* free_robots = calloc(PLAYER_ROBOTS, sizeof(int));
    for (int i = 0; i < n_robots; i += 2) {
        int x = robots[i];
        int y = robots[i + 1];
        int is_free = obstacle_map[y * W + x] != SCROOGE_INDEX;
        int is_holding_cash = cash_carried[i / 2];
        if (is_free && is_holding_cash) {
            // printf("Free Cash\n");
            int nearest_dropspot = get_nearest_dropspot(x, y, dropspots, n_dropspots, obstacle_map);
            printf("Nearest dropspot: %d\n", nearest_dropspot);
            if (nearest_dropspot == -1)
                continue;
            int dx = dropspots[nearest_dropspot * 2];
            int dy = dropspots[nearest_dropspot * 2 + 1];
            obstacle_map_2[dy * W + dx] = 66;
            obstacle_map_2[y * W + x] = 77;
            print_grid(obstacle_map_2, W, H);
            printf("Moving from robot (%d, %d) to dropspot (%d, %d)", x, y, dx, dy);
            int* move = get_a_star_move(x, y, dx, dy, obstacle_map, get_sneaky_neighbours,
                manhatten_repulsion_heuristic, has_reached_goal);
            printf("Does move equal null=%d\n", move == NULL);
            if (move != NULL) {
                action[i] = move[0];
                action[i + 1] = move[1];
                free(move);
            }
        } else if (is_free) {
            // printf("Free\n");
            free_robots[i / 2] = 1;
            n_free_robots += 1;
        } else {
            // printf("Occupied\n");
            // printf("Robot (%d, %d) is occupied by scrooge\n", x, y);
            int* move = get_a_star_move(x, y, -1, -1, obstacle_map, get_flee_neighbours,
                null_heuristic, not_watched_by_scrooge);
            // printf("Move: %d, %d\n", move[0], move[1]);
            if (move != NULL) {
                action[i] = move[0];
                action[i + 1] = move[1];
                free(move);
            }
        }
    }
    if (n_free_robots == 0 || n_cashbags == 0) {
        printf("No free robots or no cashbags\n");
        return action;
    }
    // printf("n_cashbags: %d\n", n_cashbags);
    int* distance_matrix
        = get_distance_matrix(robots, free_robots, cashbags, n_cashbags, n_robots, obstacle_map);
    // print_grid(distance_matrix, n_robots / 2, n_cashbags / 2);
    int* occupied_cashbag = calloc(n_cashbags / 2, sizeof(int));
    while (1) {
        // printf("Looping\n");
        int robot_index = -1;
        int cash_index = -1;
        int min_distance = INT_MAX;
        for (int j = 0; j < n_cashbags / 2; j++) {
            if (occupied_cashbag[j]) {
                continue;
            }
            for (int i = 0; i < PLAYER_ROBOTS; i++) {
                if (!free_robots[i]) {
                    continue;
                }
                int distance = distance_matrix[j * (n_robots / 2) + i];
                if (distance < min_distance) {
                    min_distance = distance;
                    robot_index = i;
                    cash_index = j;
                }
            }
        }
        if (min_distance == INT_MAX) {
            break;
        }
        free_robots[robot_index] = 0;
        occupied_cashbag[cash_index] = 1;
        int rx = robots[robot_index * 2];
        int ry = robots[robot_index * 2 + 1];
        int cx = cashbags[cash_index * 2];
        int cy = cashbags[cash_index * 2 + 1];
        // printf("Robot (%d,%d) is going to pick up cash at (%d, %d) with distance %d", rx, ry, cx,
        // cy, min_distance);
        int* move = get_a_star_move(rx, ry, cx, cy, obstacle_map, get_sneaky_neighbours,
            manhatten_repulsion_heuristic, has_reached_goal);
        // break;
        if (move != NULL) {
            action[robot_index * 2] = move[0];
            action[robot_index * 2 + 1] = move[1];
            free(move);
        }
        if (n_free_robots == 0) {
            break;
        } else {
            n_free_robots--;
        }
    }
    free(obstacle_map);
    free(free_robots);
    return action;
}

// Command: make helper && python main.py

// int idx = -1;
// int pqVal[MAX_QUEUE_SIZE];
// int pqPriority[MAX_QUEUE_SIZE];
// // To enqueue items as per priority
// enqueue(5, 1, &idx, pqVal, pqPriority);
// enqueue(10, 3, &idx, pqVal, pqPriority);
// enqueue(15, 4, &idx, pqVal, pqPriority);
// enqueue(20, 5, &idx, pqVal, pqPriority);
// enqueue(500, 2, &idx, pqVal, pqPriority);

// printf("Before Dequeue : \n");
// display(idx, pqVal, pqPriority);

// // Dequeue the top element
// dequeue(&idx, pqVal, pqPriority); // 20 dequeued
// dequeue(&idx, pqVal, pqPriority); // 15 dequeued

// printf("\nAfter Dequeue : \n");
// display(idx, pqVal, pqPriority);
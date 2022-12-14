#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
int OBSTACLE_INDEX = 9;
int SCROOGE_INDEX = 8;
int SCROOGE_RADIUS = 15;
int NEAR_SCROOGE_RADIUS = 6;
int ACTION_SIZE = 10;
int PLAYER_ROBOTS = 5;
int N_NEIGHBOURS = 16;
int W = 128;
int H = 128;
#define MAX_QUEUE_SIZE (128 * 128)

// returns the index of the parent node
int parent(int i) { return (i - 1) / 2; }

// return the index of the left child
int left_child(int i) { return 2 * i + 1; }

// return the index of the right child
int right_child(int i) { return 2 * i + 2; }

void swap(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

int is_cashbag_free(int cx, int cy, int* obstacle_map)
{
    int obstacle_value = obstacle_map[cy * W + cx];
    return obstacle_value != SCROOGE_INDEX && obstacle_value != NEAR_SCROOGE_RADIUS;
}

// insert the item at the appropriate position
void insert(int keys[], int values[], int key, int value, int* n)
{
    if (*n >= MAX_QUEUE_SIZE) {
        printf("%s\n", "The heap is full. Cannot insert");
        return;
    }
    // first insert the time at the last position of the array
    // and move it up
    keys[*n] = key;
    values[*n] = value;
    *n = *n + 1;

    // move up until the heap property satisfies
    int i = *n - 1;
    while (i != 0 && keys[parent(i)] < keys[i]) {
        swap(&keys[parent(i)], &keys[i]);
        swap(&values[parent(i)], &values[i]);
        i = parent(i);
    }
}

// moves the item at position i of array a
// into its appropriate position
void max_heapify(int keys[], int values[], int i, int n)
{
    // find left child node
    int left = left_child(i);

    // find right child node
    int right = right_child(i);

    // find the largest among 3 nodes
    int largest = i;

    // check if the left node is larger than the current node
    if (left <= n && keys[left] > keys[largest]) {
        largest = left;
    }

    // check if the right node is larger than the current node
    if (right <= n && keys[right] > keys[largest]) {
        largest = right;
    }

    // swap the largest node with the current node
    // and repeat this process until the current node is larger than
    // the right and the left node
    if (largest != i) {
        int temp = keys[i];
        keys[i] = keys[largest];
        keys[largest] = temp;

        int temp2 = values[i];
        values[i] = values[largest];
        values[largest] = temp2;

        max_heapify(keys, values, largest, n);
    }
}

// deletes the max item and return
int extract_max(int keys[], int values[], int* n)
{
    int max_item = values[0];

    // replace the first item with the last item
    keys[0] = keys[*n - 1];
    values[0] = values[*n - 1];
    *n = *n - 1;

    // maintain the heap property by heapifying the
    // first item
    max_heapify(keys, values, 0, *n);
    return max_item;
}

// prints the heap
void print_heap(int a[], int n)
{
    int i;
    for (i = 0; i < n; i++) {
        printf("%d\n", a[i]);
    }
    printf("\n");
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
    // #pragma omp parallel for collapse(2)
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
    int padded_radius = SCROOGE_RADIUS + 5;
    int* radius_map = get_radius_map(padded_radius);
    // #pragma omp parallel for
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
        // #pragma omp parallel for
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

int get_nearest_dropspot_all(int rx, int ry, int* dropspots, int n_dropspots, int* obstacle_map)
{
    int min_distance = INT_MAX;
    int min_index = -1;
    for (int i = 0; i < n_dropspots; i += 2) {
        int dx = dropspots[i];
        int dy = dropspots[i + 1];
        int distance = get_custom_distance(rx, ry, dx, dy);
        if (distance < min_distance) {
            min_distance = distance;
            min_index = i / 2;
        }
    }
    return min_index;
}

int get_nearest_dropspot(int rx, int ry, int* dropspots, int n_dropspots, int* obstacle_map)
{
    int min_distance = INT_MAX;
    int min_index = -1;
    for (int i = 0; i < n_dropspots; i += 2) {
        int dx = dropspots[i];
        int dy = dropspots[i + 1];
        // printf("Checking dropspot %d %d\n", dx, dy);
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

int get_nearest_cashbag(int rx, int ry, int* cashbags, int n_cashbags, int* obstacle_map)
{
    int min_distance = INT_MAX;
    int min_index = -1;
    for (int i = 0; i < n_cashbags; i += 2) {
        int dx = cashbags[i];
        int dy = cashbags[i + 1];
        // printf("Checking dropspot %d %d\n", dx, dy);
        int obstacle_value = obstacle_map[dy * W + dx];
        if (!is_cashbag_free(dx, dy, obstacle_map)) {
            continue;
        }
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

int manhatten_heuristic(int sx, int sy, int ex, int ey, int* obstacle_map)
{
    // Adding obstacle_map for encouarging the path to go around scrooges
    return get_custom_distance(sx, sy, ex, ey);
}

int* get_neighbours_base(
    int x, int y, int* obstacle_map, int (*condition)(int*, int, int, int*), int* cashbag_map)
{
    int* output = calloc(16, sizeof(int));
    // needs to be -1 from beginning
    // #pragma omp parallel for
    for (int i = 0; i < 16; i++) {
        output[i] = -1;
    }
    int index = 0;
    int all_moves[] = { 1, 0, -1, 0, 0, 1, 0, -1, 1, 1, 1, -1, -1, 1, -1, -1 };
    for (int a = 0; a < 16; a += 2) {
        int i = all_moves[a];
        int j = all_moves[a + 1];
        if (i == 0 && j == 0)
            continue;
        int x2 = x + i;
        int y2 = y + j;
        if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
            int index2 = y2 * W + x2;
            int value = obstacle_map[index2];
            if (condition(obstacle_map, x2, y2, cashbag_map)) {
                output[index] = x2;
                output[index + 1] = y2;
                index += 2;
            }
        }
    }
    return output;
}

int flee_condition(int* obstacle_map, int x, int y, int* cashbag_map)
{
    int index = y * W + x;
    int value = obstacle_map[index];
    int on_top_cashbag = cashbag_map[index];
    return !(on_top_cashbag || value == OBSTACLE_INDEX);
}

int* get_flee_neighbours(int x, int y, int* obstacle_map, int* cashbag_map)
{
    return get_neighbours_base(x, y, obstacle_map, flee_condition, cashbag_map);
}
int sneaky_condition(int* obstacle_map, int x, int y, int* cashbag_map)
{
    int index = y * W + x;
    int value = obstacle_map[index];
    return value != OBSTACLE_INDEX && value != SCROOGE_INDEX;
}

int* get_sneaky_neighbours(int x, int y, int* obstacle_map, int* cashbag_map)
{
    return get_neighbours_base(x, y, obstacle_map, sneaky_condition, cashbag_map);
}

int has_reached_goal(int sx, int sy, int ex, int ey, int* obstacle_map)
{
    return sx == ex && sy == ey;
}

int* get_a_star_move(int sx, int sy, int tx, int ty, int* obstacle_map,
    int* (*get_neighbours)(int, int, int*, int*), int (*heuristic)(int, int, int, int, int*),
    int (*is_done)(int, int, int, int, int*), int* cashbag_map)
{
    int* came_from = calloc(W * H, sizeof(int));
    int* cost_so_far = calloc(W * H, sizeof(int));
    // -- Binary heap of cells to visit
    int n = 0;
    int* keys = calloc(MAX_QUEUE_SIZE, sizeof(int));
    int* values = calloc(MAX_QUEUE_SIZE, sizeof(int));
    // --
    int start_index = sy * W + sx;
    insert(keys, values, -1, start_index, &n);
    cost_so_far[start_index] = 1;
    came_from[start_index] = start_index;
    // display(idx, pqVal, pqPriority);
    int cx;
    int cy;
    // While not empty
    while (n != 0) {
        // printf("Before");
        // display(idx, pqVal, pqPriority);
        int current = extract_max(keys, values, &n);
        // printf("Dequeued: %d, %d\n", current % W, current / W);

        // printf("After");
        // display(idx, pqVal, pqPriority);
        cx = current % W;
        cy = current / W;
        // printf("CX: %d, CY: %d\n", cx, cy);
        if (is_done(cx, cy, tx, ty, obstacle_map)) {
            break;
        }
        int* neighbours = get_neighbours(cx, cy, obstacle_map, cashbag_map);
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
                insert(keys, values, -priority, neighbour_index, &n);
                came_from[neighbour_index] = current;
            }
        }
        free(neighbours);
        // printf("Added neighbours\n");
        // display(idx, pqVal, pqPriority);
    }
    free(keys);
    free(values);
    // print_grid(came_from, W, H);
    int isEmpty = n == 0;
    if (isEmpty) {
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
    // #pragma omp parallel for
    for (int i = 0; i < PLAYER_ROBOTS * (n_cashbags / 2); i++) {
        distance_matrix[i] = INT_MAX;
    }
    // #pragma omp parallel for
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

void herding(int i, int* robots, int n_scrooges, int* used_scrooge, int* scrooges, int n_cashbags,
    int* cashbags, int* obstacle_map, int* action, int* cashbag_map, int game_ticks)
{
    int rx = robots[i * 2];
    int ry = robots[i * 2 + 1];
    // Get scrooge closest to gold
    int min_distance = INT_MAX;
    // If non is found, go to scrooge 0
    int scrooge_index = 0;
    for (int j = 0; j < n_scrooges; j += 2) {
        if (used_scrooge[j / 2]) {
            continue;
        }
        int sx = scrooges[j];
        int sy = scrooges[j + 1];
        for (int k = 0; k < n_cashbags; k += 2) {
            int cx = cashbags[k];
            int cy = cashbags[k + 1];
            int distance = get_custom_distance(sx, sy, cx, cy);
            if (distance < min_distance) {
                min_distance = distance;
                scrooge_index = j / 2;
            }
        }
    }
    int tx = scrooges[scrooge_index * 2];
    int ty = scrooges[scrooge_index * 2 + 1];
    int scrooge_closest_bottom = ty / (H / 2);
    int scrooge_closest_right = tx / (W / 2);
    int rotation_radius = 5;
    int rotation_speed = 20;
    tx = max(min(tx + cos(game_ticks / rotation_speed) * rotation_radius, W - 1), 0);
    ty = max(min(ty + sin(game_ticks / rotation_speed) * rotation_radius, H - 1), 0);
    // printf("Robobt[%d] (%d, %d) is going to pick up scrooge[%d] (%d, %d)\n", i, rx, ry,
    // scrooge_index, tx, ty);
    // int* move = get_a_star_move(rx, ry, tx, ty, obstacle_map, get_flee_neighbours,
    //     manhatten_heuristic, has_reached_goal);
    int movex = min(max(tx - rx, -1), 1);
    int movey = min(max(ty - ry, -1), 1);
    int n_index = (ry + movey) * W + (rx + movex);
    int is_cashbag_there = cashbag_map[n_index];
    int is_wall_there = obstacle_map[n_index] == OBSTACLE_INDEX;
    if (is_wall_there || is_cashbag_there) {
        int rotate_clockwise = rand() % 2;
        if (rotate_clockwise) {
            int temp = movey;
            movey = -movex;
            movex = temp;
        } else {
            int temp = movex;
            movex = -movey;
            movey = temp;
        }
    }
    used_scrooge[scrooge_index] = 1;
    action[i * 2] = movex;
    action[i * 2 + 1] = movey;
}

int* get_action(int* robots, int* scrooges, int* cashbags, int* dropspots, int* cash_carried,
    int* obstacles, int n_robots, int n_scrooges, int n_cashbags, int n_dropspots,
    int n_cash_carried, int n_obstacles, int game_ticks)
{
    // printf("Game ticks: %d\n", game_ticks);
    // printf("Float sine of game_ticks %f\n", sin(game_ticks / 10));
    srand(42); // Random seed 42
    int* action = calloc(ACTION_SIZE, sizeof(int));
    int* obstacle_map = get_obstacle_map(scrooges, obstacles, n_scrooges, n_obstacles);
    // print_grid(obstacle_map, W, H);
    int* cashbag_map = calloc(W * H, sizeof(int));
    // print_grid(obstacle_map, W, H);
    // print_grid(obstacle_map, W, H);
    // int* obstacle_map_2 = calloc(W * H, sizeof(int));
    // for (int i = 0; i < W * H; i++) {
    //     obstacle_map_2[i] = obstacle_map[i];
    // }
    int n_free_cashbags = 0;
    for (int i = 0; i < n_cashbags; i += 2) {
        int cx = cashbags[i];
        int cy = cashbags[i + 1];
        cashbag_map[cy * W + cx] = 1;
        if (is_cashbag_free(cx, cy, obstacle_map)) {
            n_free_cashbags++;
        }
    }
    // print_grid(cashbag_map, W, H);
    int n_free_robots = 0;
    int* free_robots = calloc(PLAYER_ROBOTS, sizeof(int));
    int* occupied_robots = calloc(PLAYER_ROBOTS, sizeof(int));
    int* used_scrooge = calloc(n_scrooges / 2, sizeof(int));
    int double_grab_distance = 30;
    int* occupied_cashbag = calloc(n_cashbags / 2, sizeof(int));
    // #pragma omp parallel for
    for (int i = 0; i < n_robots; i += 2) {
        int x = robots[i];
        int y = robots[i + 1];
        int is_free = obstacle_map[y * W + x] != SCROOGE_INDEX;
        int is_holding_cash = cash_carried[i / 2];
        if (!is_free && is_holding_cash) {
            int* move = get_a_star_move(x, y, -1, -1, obstacle_map, get_flee_neighbours,
                null_heuristic, not_watched_by_scrooge, cashbag_map);
            if (move != NULL) {
                action[i] = move[0];
                action[i + 1] = move[1];
                free(move);
            } else {
                // G?? IMOD DROPSPOT
            }
        } else if (is_free && is_holding_cash) {
            // printf("Free Cash\n");
            int nearest_cashbag = get_nearest_cashbag(x, y, cashbags, n_cashbags, obstacle_map);
            if (nearest_cashbag != -1 && !occupied_cashbag[nearest_cashbag]) {
                int nx = cashbags[nearest_cashbag * 2];
                int ny = cashbags[nearest_cashbag * 2 + 1];
                int ndistance = get_custom_distance(x, y, nx, ny);
                if (ndistance < double_grab_distance) {
                    int* move = get_a_star_move(x, y, nx, ny, obstacle_map, get_sneaky_neighbours,
                        manhatten_repulsion_heuristic, has_reached_goal, cashbag_map);
                    if (move != NULL) {
                        occupied_cashbag[nearest_cashbag] = 1;
                        action[i] = move[0];
                        action[i + 1] = move[1];
                        free(move);
                        continue;
                    }
                }
            }
            int nearest_dropspot = get_nearest_dropspot(x, y, dropspots, n_dropspots, obstacle_map);
            // printf("Nearest dropspot: %d\n", nearest_dropspot);
            if (nearest_dropspot == -1)
                continue;
            int dx = dropspots[nearest_dropspot * 2];
            int dy = dropspots[nearest_dropspot * 2 + 1];
            // obstacle_map_2[dy * W + dx] = 66;
            // obstacle_map_2[y * W + x] = 77;
            // print_grid(obstacle_map_2, W, H);
            // printf("Moving from robot (%d, %d) to dropspot (%d, %d)", x, y, dx, dy);
            int* move = get_a_star_move(x, y, dx, dy, obstacle_map, get_sneaky_neighbours,
                manhatten_repulsion_heuristic, has_reached_goal, cashbag_map);
            // printf("Does move equal null=%d\n", move == NULL);
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
            // int* move = get_a_star_move(x, y, -1, -1, obstacle_map, get_flee_neighbours,
            //     null_heuristic, not_watched_by_scrooge);
            // // printf("Move: %d, %d\n", move[0], move[1]);
            // if (move != NULL) {
            //     action[i] = move[0];
            //     action[i + 1] = move[1];
            //     free(move);
            // }
            occupied_robots[i / 2] = 1;
        }
    }
    // ***** hvis n_free_robots > n_free_cashbags, s?? skal vi flygte med n_free_cashbags -
    // n_free_robots robotter
    int lacking_robots = n_free_cashbags - n_free_robots;
    // printf("Lacking robots: %d\n", lacking_robots);
    // printf("Occupied robots: %d %d %d %d %d\n", occupied_robots[0], occupied_robots[1],
    //     occupied_robots[2], occupied_robots[3]);
    for (int i = 0; i < PLAYER_ROBOTS; i++) {
        if (occupied_robots[i] == 0) {
            continue;
        }
        if (lacking_robots > 0) {
            int rx1 = robots[i * 2];
            int ry1 = robots[i * 2 + 1];
            int* move = get_a_star_move(rx1, ry1, -1, -1, obstacle_map, get_flee_neighbours,
                null_heuristic, not_watched_by_scrooge, cashbag_map);
            if (move != NULL) {
                action[i * 2] = move[0];
                action[i * 2 + 1] = move[1];
                lacking_robots--;
                free(move);
            }
        } else {
            herding(i, robots, n_scrooges, used_scrooge, scrooges, n_cashbags, cashbags,
                obstacle_map, action, cashbag_map, game_ticks);
        }
    }
    // if (n_free_robots == 0 || n_cashbags == 0) {
    //     // printf("No free robots or no cashbags\n");
    //     return action;
    // }
    // printf("n_cashbags: %d\n", n_cashbags);
    int* distance_matrix
        = get_distance_matrix(robots, free_robots, cashbags, n_cashbags, n_robots, obstacle_map);
    // print_grid(distance_matrix, n_robots / 2, n_cashbags / 2);
    while (1) {
        // printf("Looping\n");
        int robot_index = -1;
        int cash_index = -1;
        int min_distance = INT_MAX;
        int second_min_distance = INT_MAX;
        int second_cash_index = -1;
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
                    second_min_distance = min_distance;
                    second_cash_index = cash_index;
                    min_distance = distance;
                    robot_index = i;
                    cash_index = j;
                } else if (distance < second_min_distance) {
                    second_min_distance = distance;
                    second_cash_index = j;
                }
            }
        }
        // printf("Min distance: %d, Index: %d\n", min_distance, cash_index);
        // printf("Second min distance: %d, Index: %d\n", second_min_distance, second_cash_index);
        if (min_distance == INT_MAX) {
            break;
        }
        if (min_distance < double_grab_distance && second_min_distance < double_grab_distance) {
            occupied_cashbag[second_cash_index] = 1;
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
            manhatten_repulsion_heuristic, has_reached_goal, cashbag_map);
        // break;
        if (move != NULL) {
            action[robot_index * 2] = move[0];
            action[robot_index * 2 + 1] = move[1];
            free(move);
        }
        n_free_robots--;
    }
    // printf("Free robots: %d %d %d %d %d\n", free_robots[0], free_robots[1], free_robots[2],
    //     free_robots[3], free_robots[4]);
    for (int i = 0; i < PLAYER_ROBOTS; i++) {
        if (!free_robots[i]) {
            continue;
        }
        herding(i, robots, n_scrooges, used_scrooge, scrooges, n_cashbags, cashbags, obstacle_map,
            action, cashbag_map, game_ticks);
    }
    // printf("\n");
    free(obstacle_map);
    free(free_robots);
    return action;
}

// Command: make helper && python main.py
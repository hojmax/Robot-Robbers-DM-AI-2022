#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* get_action(int* robots, int* scrooges, int* cashbags, int* dropspots, int* cash_carried,
    int* obstacles, int len_robots, int len_scrooges, int len_cashbags, int len_dropspots,
    int len_cash_carried, int len_obstacles, int game_ticks)
{
    int* actions = calloc(len_robots, sizeof(int));
    return actions;
}

// Command: make helper && python main.py
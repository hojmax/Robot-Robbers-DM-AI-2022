#ifndef A_STAR_H_INCLUDED
#define A_STAR_H_INCLUDED

int* get_a_star_move(int, int, int, int, int* (*)(int, int, StateInfo),
    int (*)(int, int, int, int, StateInfo), int (*)(int, int, int, int, StateInfo), StateInfo);
int* get_flee_neighbours(int, int, StateInfo);
int not_watched_is_done(int, int, int, int, StateInfo);
int null_heuristic(int, int, int, int, StateInfo);

#endif
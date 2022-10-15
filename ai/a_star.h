#ifndef A_STARR_H_INCLUDED
#define A_STARR_H_INCLUDED

int* get_a_star_move(int, int, int, int, int* (*)(int, int, int**),
    int (*)(int, int, int, int, int**), int (*)(int, int, int, int, int**), int**);

#endif
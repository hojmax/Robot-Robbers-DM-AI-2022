#ifndef AI_H_INCLUDED
#define AI_H_INCLUDED

typedef struct StateInfo {
    int* obstacle_map;
    int* padded_obstacle_map;
    int* cashbag_map;
    int* scrooge_map;
    int* scrooge_radius_map;
    int* dropspot_map;
} StateInfo;

#endif
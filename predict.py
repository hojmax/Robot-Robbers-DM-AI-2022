import numpy as np
import ctypes
ai = ctypes.CDLL('./ai.so')
ai.get_action.restype = ctypes.POINTER(ctypes.c_int)


def convert_list(lst):
    return (ctypes.c_int * len(lst))(*lst)


def get_action(state, game_ticks):
    robots, scrooges, cashbags, dropspots, cash_carried, obstacles = extract_information(
        state
    )
    action = ai.get_action(
        convert_list(robots),
        convert_list(scrooges),
        convert_list(cashbags),
        convert_list(dropspots),
        convert_list(cash_carried),
        convert_list(obstacles),
        len(robots),
        len(scrooges),
        len(cashbags),
        len(dropspots),
        len(cash_carried),
        len(obstacles),
        game_ticks
    )
    return [action[i] for i in range(10)]


def extract_information(state):
    robots = np.array(
        [[x, y]
         for (x, y, w, h) in state[0] if x >= 0 and y >= 0]
    ).flatten()
    scrooges = np.array(
        [[x, y] for (x, y, w, h) in state[1] if x >= 0 and y >= 0]
    ).flatten()
    cashbags = np.array(
        [[x, y] for (x, y, w, h) in state[2]
         if x >= 0 and y >= 0]
    ).flatten()
    dropspots = np.array(
        [[x, y] for (x, y, w, h) in state[3] if x >= 0 and y >= 0]
    ).flatten()
    obstacles = np.array(
        [[x, y, w, h] for x, y, w, h in state[4] if x >= 0 and y >= 0]
    ).flatten()
    cash_carried = [row[0] for row in state[5] if row[0] >= 0]

    return robots, scrooges, cashbags, dropspots, cash_carried, obstacles

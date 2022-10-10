from environment import RobotRobbersEnv
import numpy as np
import ctypes
helpers = ctypes.CDLL('./helper.so')
# --- Standard
# robots = [19, 25, 116, 42, 80, 96, 65, 76, 87, 61]
# scrooges = [18, 24, 103, 97, 98, 27, 64, 47, 28, 45, 112, 17, 122, 16]
# cashbags = [84, 4, 22, 63, 12, 107, 85, 78, 77, 0]
# dropspots = [14, 78]
# cash_carried = [0, 0, 0, 0, 0]
# obstacles = [0, 99, 19, 2, 98, 53, 11, 8]

# --- Bounding-boxes outside screen
# robots = [75, 119, 60, 88, 57, 90, 47, 69, 73, 37]
# scrooges = [83, 54, 46, 55, 103, 19, 9, 33, 88, 4, 93, 34, 67, 88]
# cashbags = []
# dropspots = []
# obstacles = [85, 74, 18, 16, 127, 18, 13, 16, 3, 127, 19, 2, 111, 65, 1, 8]
# cash_carried = [0, 0, 0, 0, 0]

# -- High action
# robots = [12, 14, 73, 12, 105, 45, 127, 65, 85, 36]
# scrooges = [102, 46, 124, 48, 118, 66, 87, 9, 97, 28, 101, 95, 85, 79]
# cashbags = [30, 40, 65, 117, 86, 112, 67, 40, 19, 60]
# dropspots = [53, 97, 6, 113, 120, 122]
# obstacles = [22, 99, 8, 19, 73, 45, 6, 15, 100, 11, 12, 18]
# cash_carried = [0, 0, 0, 0, 0]

# -- Standing still
# robots = [47, 24, 47, 82, 68, 113, 50, 36, 78, 123]
# scrooges = [28, 33, 106, 16, 11, 70, 63, 19, 52, 51, 97, 54, 120, 74]
# cashbags = [77, 49, 59, 42, 107, 1]
# dropspots = [78, 123, 3, 30, 16, 34]
# obstacles = [57, 120, 16, 14, 55, 106, 1, 10, 47, 90, 7, 14, 26, 42, 12, 14]
# cash_carried = [1, 0, 0, 1, 0]

helpers.get_action.restype = ctypes.POINTER(ctypes.c_int)


def convert_list(lst):
    return (ctypes.c_int * len(lst))(*lst)


def get_action(state):
    robots, scrooges, cashbags, dropspots, cash_carried, obstacles = extract_information(
        state
    )
    action = helpers.get_action(
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
        len(obstacles)
    )
    output = []
    for i in range(10):
        output.append(action[i])
    return output


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
    # print('robots =', list(robots))
    # print('scrooges =', list(scrooges))
    # print('cashbags =', list(cashbags))
    # print('dropspots =', list(dropspots))
    # print('obstacles =', list(obstacles))
    # print('cash_carried =', list(cash_carried))
    return robots, scrooges, cashbags, dropspots, cash_carried, obstacles


if __name__ == '__main__':
    # import pygame
    env = RobotRobbersEnv()
    state = env.reset()

    # --- Static testing
    # for _ in range(1):
    #     action = get_action(state)
    #     print('Action:', action)
    #     state, reward, done, info = env.step(action)

    # --- Dynamic testing
    # clock = pygame.time.Clock()
    while True:
        # clock.tick(24)
        action = get_action(state)
        # print('Action:', action)
        state, reward, done, info = env.step(action)
        env.render()

from environment import RobotRobbersEnv
import numpy as np
# import pygame
import ctypes
helpers = ctypes.CDLL('./helper.so')
# robots = [19, 25, 116, 42, 80, 96, 65, 76, 87, 61]
# scrooges = [18, 24, 103, 97, 98, 27, 64, 47, 28, 45, 112, 17, 122, 16]
# cashbags = [84, 4, 22, 63, 12, 107, 85, 78, 77, 0]
# dropspots = [14, 78]
# cash_carried = [0, 0, 0, 0, 0]
# obstacles = [0, 99, 19, 2, 98, 53, 11, 8]

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
    print('Robots', robots)
    print('Scrooges', scrooges)
    print('Cashbags', cashbags)
    print('Dropspots', dropspots)
    print('Obstacles', obstacles)
    print('Cash carried', cash_carried)
    return robots, scrooges, cashbags, dropspots, cash_carried, obstacles


if __name__ == '__main__':
    env = RobotRobbersEnv()
    state = env.reset()

    for _ in range(1):
        action = get_action(state)
        print('Action:', action)
        state, reward, done, info = env.step(action)

    # clock = pygame.time.Clock()
    # while True:
    #     clock.tick(24)
    #     print('Trying to get action')
    #     action = get_action(state)
    #     state, reward, done, info = env.step(action)
    # env.render()
    # break

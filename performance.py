from environment import RobotRobbersEnv
from predict import get_action
import time
import random
import pygame

if __name__ == '__main__':
    env = RobotRobbersEnv()
    n_steps = 6300
    n_games = 10
    average_game_reward = 0
    seeds = [2603, 662763, 918254, 148000, 184260, 715732]
    for game in range(n_games):
        # seed = random.randint(0, 1000000)
        seed = seeds[game]
        state = env.reset(seed)
        reward_sum = 0
        start_time = time.time()
        for step in range(n_steps):
            if game == len(seeds)-1:
                clock = pygame.time.Clock()
                env.render()
            action = get_action(state)
            state, reward, done, info = env.step(action)
            reward_sum += reward
        print(f'Info reward:', info['total_reward'])
        end_time = time.time()
        print(
            f'Game: {game}, Total Reward: {reward_sum}, Steps: {n_steps}, Time: {end_time - start_time:.2f}s, Seed: {seed}'
        )
        average_game_reward += reward_sum
    print(
        f'Average Game Reward ({n_games} games): {average_game_reward / n_games}'
    )


# Command:
# $ export OMP_NUM_THREADS=2
# $ make helper && time python benchmark.py

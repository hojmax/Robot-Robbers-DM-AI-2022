from environment import RobotRobbersEnv
from predict import get_action
import time

if __name__ == '__main__':
    env = RobotRobbersEnv()
    n_steps = 6300
    n_games = 10
    average_game_reward = 0
    for game in range(n_games):
        state = env.reset()
        reward_sum = 0
        start_time = time.time()
        for step in range(n_steps):
            action = get_action(state)
            state, reward, done, info = env.step(action)
            reward_sum += reward
        end_time = time.time()
        print(
            f'Game: {game}, Total Reward: {reward_sum}, Time: {end_time - start_time:.2f}s'
        )
        average_game_reward += reward_sum
    print(
        f'Average Game Reward ({n_games} games): {average_game_reward / n_games}'
    )


# Command:
# $ export OMP_NUM_THREADS=2
# $ make helper && time python benchmark.py

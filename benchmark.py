from environment import RobotRobbersEnv
from predict import get_action

if __name__ == '__main__':
    env = RobotRobbersEnv()
    state = env.reset(42)
    n_steps = 10000
    for i in range(n_steps):
        action = get_action(state)
        state, reward, done, info = env.step(action)

# Command: make helper && time python benchmark.py

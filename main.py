from environment import RobotRobbersEnv
from predict import get_action

if __name__ == '__main__':

    env = RobotRobbersEnv()

    # state = env.reset()
    # for step in range(1):
    #     action = get_action(state)
    #     print('Action:', action)
    #     state, reward, done, info = env.step(action)

    # # --- Static testing
    # for ep in range(100):
    #     state = env.reset()
    #     for step in range(6000):
    #         action = get_action(state)
    #         print('Action:', action)
    #         state, reward, done, info = env.step(action)
    #         print(f'Episode: {ep}, Step: {step}, Total Reward: {info["total_reward"]}')

    # --- Dynamic testing
    import pygame
    state = env.reset()
    tick = 0
    clock = pygame.time.Clock()
    while True:
        clock.tick(24)
        action = get_action(state, tick)
        # print('Action:', action)
        state, reward, done, info = env.step(action)
        tick += 1
        env.render()

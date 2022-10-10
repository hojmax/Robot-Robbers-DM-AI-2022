from environment import RobotRobbersEnv
from predict import get_action

if __name__ == '__main__':
    import pygame
    env = RobotRobbersEnv()

    # # --- Static testing
    # for ep in range(100):
    #     state = env.reset()
    #     for step in range(6000):
    #         action = get_action(state)
    #         print('Action:', action)
    #         state, reward, done, info = env.step(action)
    #         print(f'Episode: {ep}, Step: {step}, Total Reward: {info["total_reward"]}')

    # --- Dynamic testing
    state = env.reset()
    clock = pygame.time.Clock()
    while True:
        clock.tick(24)
        action = get_action(state)
        # print('Action:', action)
        state, reward, done, info = env.step(action)
        env.render()

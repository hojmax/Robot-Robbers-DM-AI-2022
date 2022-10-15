from env.environment import RobotRobbersEnv
from predict import get_action
import pygame

env = RobotRobbersEnv()
state = env.reset()
clock = pygame.time.Clock()
tick = 0

while True:
    clock.tick(24)
    action = get_action(state, tick)
    state, _, _, _ = env.step(action)
    tick += 1
    env.render()

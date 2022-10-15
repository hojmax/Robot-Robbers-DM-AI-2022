from env.environment import RobotRobbersEnv
from predict import get_action
import pygame

env = RobotRobbersEnv()
seed = 100
state = env.reset(seed)
clock = pygame.time.Clock()
tick = 0
end_tick = 1000

while True:
    clock.tick(24)
    action = get_action(state, tick, end_tick)
    state, _, _, _ = env.step(action)
    tick += 1
    env.render()

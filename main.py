import sys
import pygame as pg
from math import sqrt

class Car():
    def __init__(self, color):
        self.color = color
        self.x, self.y = 0.0,0.0
        self.sizex, self.sizey = 50.0,50.0
        self.vx, self.vy = 0.0,0.0
        self.surface = pg.image.load("intro_ball.gif")
        self.body = self.surface.get_rect()
        self.body.center = (self.sizex/2, self.sizey/2)


pg.init()
pg.display.set_caption('Fast or Furious')

size = width, height = 800, 500
speed = [1, 1]
black = 0, 0, 0

screen = pg.display.set_mode(size)
cars = [Car((255,0,0))]
background = pg.image.load("intro_ball.gif")
# ballrect = ball.get_rect()
clock = pg.time.Clock()
while 1:
    screen.fill(black)
    # screen.blit(background, (width/2-background.get_width()/2,height/2-background.get_height()/2))

    for event in pg.event.get():
        if event.type == pg.QUIT: sys.exit()

    for car in cars:
        screen.blit(car.surface, car.body)

        key_input = pg.key.get_pressed()
        car.vx = 0
        car.vy = 0
        if key_input[pg.K_LEFT]:
            car.vx = -speed[0]
        if key_input[pg.K_UP]:
            car.vy = -speed[1]
        if key_input[pg.K_RIGHT]:
            car.vx = speed[0]
        if key_input[pg.K_DOWN]:
            car.vy = speed[1]
        if sum((
            key_input[pg.K_DOWN],
            key_input[pg.K_UP],
            key_input[pg.K_LEFT],
            key_input[pg.K_RIGHT])) > 1:
            car.vx /= sqrt(2)
            car.vy /= sqrt(2)
        dt = clock.tick(60)
        car.body.x += car.vx * dt
        car.body.y += car.vy * dt
        car.body.x = max(min(car.body.x,width-car.body.width), 0.)
        car.body.y = max(min(car.body.y,height-car.body.height), 0.)

        # carrect
    # ballrect = ballrect.move(speed)
    # if ballrect.left < 0 or ballrect.right > width:
    #     speed[0] = -speed[0]
    # if ballrect.top < 0 or ballrect.bottom > height:
    #     speed[1] = -speed[1]

    pg.display.flip()

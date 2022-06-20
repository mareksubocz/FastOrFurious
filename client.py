import pickle
import socket
import struct
import sys
import math
from math import atan

class Response:
    def __init__(self, gas: int, rotate: int, boost: bool):
        self.gas = gas
        self.rotate = rotate
        self.boost = boost

class PlayerState:
    """
    It needs to be last in the pack.
    """
    def __init__(self, gameState, numOfPlayers, checkpointsX, checkpointsY) -> None:
        playerStates = [gameState[i*8:(i+1)*8] for i in range(numOfPlayers)]
        self.posx = playerStates[-1][0]
        self.posy = playerStates[-1][1]
        self.velx = playerStates[-1][2]
        self.vely = playerStates[-1][3]
        self.rot = playerStates[-1][4]
        self.lap = changeEndian(playerStates[-1][5])
        self.chkpt = changeEndian(playerStates[-1][6])
        self.health = changeEndian(playerStates[-1][7])

        self.checkpointsX = checkpointsX
        self.checkpointsY = checkpointsY
        #find the closest's num
        closest_dist = float('inf')
        closest_i = -1
        for i in range(numOfPlayers-1):
            pos = (playerStates[i][0], playerStates[i][1])
            distance = math.dist(pos, [self.posx, self.posy])
            if distance < closest_dist:
                closest_dist = distance
                closest_i = i

        self.closestPosx = playerStates[closest_i][0]
        self.closestPosy = playerStates[closest_i][1]
        self.closestVelx = playerStates[closest_i][2]
        self.closestVely = playerStates[closest_i][3]
        self.closestRot = playerStates[closest_i][4]
        self.closestLap = changeEndian(playerStates[closest_i][5])
        self.closestChkpt = changeEndian(playerStates[closest_i][6])
        self.closestHealth = changeEndian(playerStates[closest_i][7])
        self.calculate_relatives()

    def to_array(self):
        return [
        self.posx,
        self.posy,
        self.velx,
        self.vely,
        self.rot,
        self.lap,
        self.chkpt,
        self.health,
        self.relposx,
        self.relposy,
        self.relvelx,
        self.relvely,
        self.relrot,
        self.enemyHealth,
        self.relCheckpointPosx,
        self.relCheckpointPosy,
        self.relCheckpointRot,
        ]

    def rad2deg(self, angle):
        return angle * 180.0 / math.pi;

#relposx, relposy, relvelx, relvely, relrot, enemyHealth, relCheckpointPosx, relCheckpointPosy, relCheckpointRot
    def __vectorRotation(self, a, b):
        if b == 0:
            b = 0.01
        if a == 0:
            a = 0.01
        if b >= 0 and a >= 0:
            return self.rad2deg(atan(a / b));
        elif (b < 0 and a >= 0):
            return 180 - self.rad2deg(atan(a / -b));
        elif b < 0 and a < 0:
            return 180 + self.rad2deg(atan(-a / -b));
        else:
            return 360 - self.rad2deg(atan(-a / b));


    def calculate_relatives(self):
        #TODO: actually calculate
        self.relposx = self.closestPosx - self.posx
        self.relposy = self.closestPosy - self.posy
        self.relvelx = self.closestVelx - self.velx
        self.relvely = self.closestVely - self.vely
        self.relrot = self.__vectorRotation(self.relposx, self.relposy) - self.rot
        self.enemyHealth = self.closestHealth
        self.relCheckpointPosx = self.checkpointsX[self.chkpt] - self.posx
        self.relCheckpointPosy = self.checkpointsY[self.chkpt] - self.posy
        self.relCheckpointRot = self.__vectorRotation(self.relCheckpointPosx, self.relCheckpointPosy) - self.rot

def changeEndian(n):
    bytes = (n).to_bytes(4, 'big')
    return int.from_bytes(bytes, 'little')

def connectToServer(s, serverIP, serverPort):
    serverIP = sys.argv[1]
    serverPort = int(sys.argv[2])
    myAddress, myPort = s.getsockname()
    print(myAddress, myPort)
    packet = struct.pack('>20sI', myAddress.encode('ascii'), myPort)
    s.sendto(packet, (serverIP, serverPort))

    packet = s.recvfrom(struct.calcsize('ffffffiiiiffffff'))
    frontFriction, sideFriction, acc, dec, rotationSpeed, maxVelocity,\
    timeout, framesPerAnswer, numOfPlayers, numOfCheckpoints, ch1x, ch1y,\
    ch2x, ch2y, ch3x, ch3y = struct.unpack('ffffffiiiiffffff', packet[0])
    timeout = changeEndian(timeout)
    framesPerAnswer = changeEndian(framesPerAnswer)
    numOfPlayers = changeEndian(numOfPlayers)
    numOfCheckpoints = changeEndian(numOfCheckpoints)
    checkpointsX = [ch1x, ch2x, ch3x]
    checkpointsY = [ch1y, ch2y, ch3y]
    return numOfPlayers, checkpointsX, checkpointsY

def waitForGameState(s, numOfPlayers, checkpointsX, checkpointsY):
    packet = s.recvfrom(struct.calcsize("fffffiii"*numOfPlayers))
    gameState = struct.unpack("fffffiii"*numOfPlayers, packet[0])
    # print('gamestate:', gameState[8:16])
    myState = PlayerState(gameState, numOfPlayers, checkpointsX, checkpointsY)
    return myState

def calculateResponse(gameState, model):
    response = Response(*model.predict([gameState.to_array()])[0])
    response.gas *= 100
    # print(f"{response.gas=}")
    # print(f"{response.rotate=}")
    return response

def sendResponse(s, response):
    serverIP = sys.argv[1]
    serverPort = int(sys.argv[2])
    packet = struct.pack('>iib', round(response.gas), round(response.rotate), bool(response.boost))
    s.sendto(packet, (serverIP, serverPort))


if __name__ == "__main__":
    with open('model.pickle', 'rb') as file:
        clf = pickle.load(file)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind(('localhost', 0))
        numOfPlayers, checkpointsX, checkpointsY = connectToServer(s, sys.argv[1], sys.argv[2])
        while(True):
            gameState = waitForGameState(s, numOfPlayers, checkpointsX, checkpointsY)
            response = calculateResponse(gameState, clf)
            sendResponse(s, response)

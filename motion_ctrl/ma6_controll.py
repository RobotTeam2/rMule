import time
import sys

scenario = [
            [["right"]],
            [["forward",1],["back",2],["forward",3],["back",4],["forward",5],["back",6]],
            [["left"]],
            [["back",1],["forward",2],["back",3],["forward",4],["back",5],["forward",6]]
           ]


def arduino_command(a):
    print("arduino")
    if a[0] == "forward":
        print("legM:id,%1d:xmm,0" % a[1])
    elif  a[0] == "back":
        print("legM:id,%1d:xmm,150" % a[1])
    else:
        pass

def stm_command(a):
    print("stm")
    if a[0] == "height":
        print("height:%1d" % a[1])
    elif a[0] == "right":
        print("right")
    elif a[0] == "left":
        print("left")
    else:
        pass

def player(scenario):
    for motion in scenario:
        print(motion) 
        for command in motion:
            if command[0] == "right":
                stm_command(command)
            elif  command[0] == "left":
                stm_command(command)
            elif  command[0] == "forward":
                arduino_command(command)
            elif  command[0] == "back":
                arduino_command(command)
            else:
                pass

        time.sleep(1.0)

def setup():
    # TO DO 
    pass


def main():

    setup()

    i = 0

    while 1:
        player(scenario)
        print("---------")
        time.sleep(2)

        i = i + 1
        if (i == 10):
            sys.exit()
        

if __name__ == '__main__':
    main()

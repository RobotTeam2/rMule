import time

key_command_map = {
    b'\t':["command",["left"]],
    b'/':["command",["right"]],
    b'*':["scenario",["walk"]],
    b'\x7f':["scenario",["back"]],

    b'7':["motor_command",["up"]],
    b'8':["motor_id",["0"]],
    b'9':["motor_id",["3"]],
    b'-':["motor_command",["move", 0]],

    b'4':["motor_command",["down"]],
    b'5':["motor_id",["1"]],
    b'6':["motor_id",["4"]],
    b'+':["motor_command",["move",100]],
    
    b'1':["command",["init"]],  
    b'2':["motor_id",["2"]],      
    b'3':["motor_id",["5"]],     
    b'\r':["command",["stop"]],

#    b'0':["command",["reset"]], 
    b'.':["motor_id",["all"]]

}

class _Getch:
    """Gets a single character from standard input.  Does not echo to the
screen."""
    def __init__(self):
        try:
            self.impl = _GetchWindows()
        except ImportError:
            self.impl = _GetchUnix()

    def __call__(self): return self.impl()


class _GetchUnix:
    def __init__(self):
        import tty, sys

    def __call__(self):
        import sys, tty, termios
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch.encode('utf-8')


class _GetchWindows:
    def __init__(self):
        import msvcrt

    def __call__(self):
        import msvcrt
        return msvcrt.getch()

# Collect events until released
if __name__ == '__main__':

    print("start listener")
    motor_id = ""
    getch = _Getch()
    
    while True:
        key = getch()
        print('{0} pressed'.format(key))

        if key == b'\x03':
            break

        if key in key_command_map:
            command = key_command_map[key]
            if command[0] == "scenario":
                print("scenario {}".format(command[1]))
                motor_id = ""
            elif command[0] == "command":
                print("command {}".format(command[1]))
                motor_id = ""
            elif command[0] == "motor_command":
                print("motor_command {}".format(command[1]))
                motor_command = command[1]
                if len(motor_id) > 0:
                    print("execute motor command {}:{} ".format(command[1],motor_id))
                    motor_id = ""
                    time.sleep(1)
            elif command[0] == "motor_id":
                print("motor_id {}".format(command[1]))
                motor_id = command[1]
                
            else:
                pass               
        

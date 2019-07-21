from pynput.keyboard import Key, KeyCode, Listener
import queue

key_command_map = {
    Key.tab:["command",["left"]],
    KeyCode.from_char('/'):["command",["right"]],
    KeyCode.from_char('*'):["scenario",["walk"]],
    Key.backspace:["scenario",["back"]],

    KeyCode.from_char('7'):["motor_command",["up"]],
    KeyCode.from_char('8'):["motor_id",["0"]],
    KeyCode.from_char('9'):["motor_id",["3"]],
    KeyCode.from_char('-'):["motor_command",["move", 0]],

    KeyCode.from_char('4'):["motor_command",["down"]],
    KeyCode.from_char('5'):["motor_id",["1"]],
    KeyCode.from_char('6'):["motor_id",["4"]],
    KeyCode.from_char('+'):["motor_command",["move",100]],
    
    KeyCode.from_char('1'):["motor_id",["all"]],  
    KeyCode.from_char('2'):["motor_id",["2"]],      
    KeyCode.from_char('3'):["motor_id",["5"]],     
    Key.enter:["command",["stop"]],

    KeyCode.from_char('0'):["command",["init"]], 
    KeyCode.from_char('.'):["command",["reset"]]
}

def on_press(key):
    command_queue.put(key)

def on_release(key):
    print('{0} release'.format(key))
    if key == Key.esc:
        # Stop listener
        return False


# Collect events until released
if __name__ == '__main__':
    command_queue = queue.Queue()

    with Listener(
            on_press=on_press,
            on_release=on_release) as listener:

        print("start listener")
        motor_command = ""
        while True:
            key = command_queue.get()
            print('{0} pressed'.format(key))

            if key in key_command_map:
                command = key_command_map[key]
                if command[0] == "scenario":
                    print("scenario {}".format(command[1]))
                elif command[0] == "command":
                    print("command {}".format(command[1]))
                elif command[0] == "motor_command":
                    print("motor_command {}".format(command[1]))
                    motor_command = command[1]
                    print("wait motor_id")
                elif command[0] == "motor_id":
                    print("motor_id {}".format(command[1]))
                    
                    if len(motor_command) > 0:
                        print("command {} {}".format(motor_command,command[1]))
                        motor_command = ""
                    else:
                        print("motor_command missing")

                else:
                    pass               
            
            if key == Key.esc:
                break 

        listener.join()

    
    

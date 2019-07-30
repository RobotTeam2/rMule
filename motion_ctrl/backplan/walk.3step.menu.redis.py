#!/usr/bin/python3

import time
import sys
import glob
#import serial
import re
import threading
import queue
import os
from logging import getLogger, StreamHandler, FileHandler, Formatter, DEBUG

import redis


logger = getLogger(__name__)
logger.setLevel(DEBUG)

stream_formatter = Formatter('%(message)s')
stream_handler = StreamHandler()
stream_handler.setLevel(DEBUG)
stream_handler.setFormatter(stream_formatter)
logger.addHandler(stream_handler)

os.makedirs("./log",exist_ok=True)
log_file_name = "./log/log-" + time.strftime("%Y%m%d-%H%M%S", time.strptime(time.ctime()))+".txt"
file_handler = FileHandler(log_file_name)
file_handler.setLevel(DEBUG)
file_formatter = Formatter('[%(asctime)s] %(message)s')
file_handler.setFormatter(file_formatter)
logger.addHandler(file_handler)
logger.propagate = False


key_command_map = {
    b'\t':["motion",[["left"]]],
    b'/':["motion",[["right"]]],
    b'*':["scenario",["walk"]],
    b'\x08':["scenario",["back"]], # Windows
    b'\x7f':["scenario",["back"]], # Linux

    b'7':["motor_command",["up"]],
    b'8':["motor_id",0],
    b'9':["motor_id",3],
    b'-':["motor_command",["move", 0]],

    b'4':["motor_command",["down"]],
    b'5':["motor_id",1],
    b'6':["motor_id",4],
    b'+':["motor_command",["move",100]],
    
    b'1':["motion",[["stm_init"]]],  
    b'2':["motor_id",2],      
    b'3':["motor_id",5],     
    b'\r':["command",["stop"]],

    b'0':["command",["clear"]], 
    b'.':["motor_id",999],

    b'\x1b':["escape"],
    b'[':["escape"]
}

linux_esc_key_command_map = {

    b'H':["motor_command",["up"]],
    b'A':["motor_id",0],
    b'5':["motor_id",3],

    b'D':["motor_command",["down"]],
    b'E':["motor_id",1],
    b'C':["motor_id",4],
    
    b'F':["motion",[["stm_init"]]],  
    b'B':["motor_id",2],      
    b'6':["motor_id",5],     

    b'2':["command",["clear"]], 
    b'3':["motor_id",999],

    b'\x1b':["escape"],
    b'[':["escape"]
}

arduino_available = False
stm_available = False
legs = 0

scenario_repeat = 3
motor_height = []
motor_id_mapping = {}
id_motor_mapping = {}
default_motor_id_mapping_2legs = {0:"2",1:"5"}

#
#  2 legs (2番目と3番目のArduinoを外した状態)
#
#         Front
#        +-----+
#  0:"2" |     | 2:"5"
#        +-----+
#         Back
#

default_motor_id_mapping_4legs = {0:"2",1:"3",2:"5",3:"6"}

#
#  4 legs (3番目のArduinoを外した状態)
#
#         Front
#        +-----+
#  0:"2" |     | 2:"5"
#  1:"3" |     | 3:"6"
#        +-----+
#         Back
#
#  right: 0:"2",4:"6",2:"4"
#  left : 3:"5",1:"3",5:"7"
#  

default_motor_id_mapping_6legs = {0:"2",1:"3",2:"4",3:"5",4:"6",5:"7"}

#
#  6 legs
#
#         Front
#        +-----+
#  0:"2" |     | 3:"5"
#  1:"3" |     | 4:"6"
#  2:"4" |     | 5:"7"
#        +-----+
#         Back
#
#  right: 0:"2",4:"6",2:"4"
#  left : 3:"5",1:"3",5:"7"
#  

arduino_id_mapping = {}


'''
scenario_walk = [

    [["right"]],

    [["wait",2.0]],

    [["move",0,0,1], 
     ["move",4,0,1],
     ["move",2,0,1]],

    [["wait",1.0]],

    [["move",3,100,1],
     ["move",1,100,1],
     ["move",5,100,1]],

    [["wait",1.0]],

    [["left"]],

    [["wait",2.0]],

    [["move",3,0,1],
     ["move",1,0,1],
     ["move",5,0,1]],

    [["wait",1.0]],

    [["move",0,100,1],
     ["move",4,100,1],
     ["move",2,100,1]],

    [["wait",1.0]]

]
'''

left_front_earth = [
  ["move",1,100,1],
  ["move",3,100,1],
  ["move",5,100,1],
]

left_back_earth = [
  ["move",1,0,1],
  ["move",3,0,1],
  ["move",5,0,1],
]

left_front_air = [
  ["move",1,100,1],
  ["move",3,100,1],
  ["move",5,100,1],
]

left_back_air = [
  ["move",1,0,1],
  ["move",3,0,1],
  ["move",5,0,1],
]


right_front_earth = [
  ["move",0,100,1],
  ["move",2,100,1],
  ["move",4,100,1],
]

right_back_earth = [
  ["move",0,0,1],
  ["move",2,0,1],
  ["move",4,0,1],
]

right_front_air = [
  ["move",0,100,1],
  ["move",2,100,1],
  ["move",4,100,1],
]

right_back_air = [
  ["move",0,0,1],
  ["move",2,0,1],
  ["move",4,0,1],
]


wait_space = 2.0

# walk in 3 step.
scenario_walk = [
#   move all leg to front by air.
    [["right"]],
    [["wait",wait_space]],
    left_front_air,
    [["wait",wait_space]],
    [["left"]],
    [["wait",wait_space]],
    right_front_air,
    [["wait",wait_space]],
#   move short down all legs.
    [["home"]],
    [["wait",wait_space]],
#   move short down all legs.
    left_back_earth,
    right_back_earth,
    [["home"]],    
    [["wait",1.0]],
]


'''
scenario_back = [

    [["left"]],

    [["wait",5.0]],

    [["move",0,0,1],  ["move",3,0,0],
     ["move",1,0,0],  ["move",4,0,1],
     ["move",2,0,1],  ["move",5,0,0]],

    [["wait",5.0]],

    [["right"]],

    [["wait",5.0]],

    [["move",0,100,0],  ["move",3,100,1],
     ["move",1,100,1],  ["move",4,100,0],
     ["move",2,100,0],  ["move",5,100,1]],

    [["wait",5.0]]
]
'''
scenario_back = [
#   move all leg to front by air.
    [["right"]],
    [["wait",1.0]],
    left_back_air,
    [["wait",1.0]],
    [["left"]],
    [["wait",1.0]],
    right_back_air,
    [["wait",1.0]],
#   move short down all legs.
    [["alldown"]],
    [["wait",1.0]],
#   move short down all legs.
    left_front_earth,
    right_front_earth,
    [["wait",1.0]]
]


arduino_ports = []
stm_ports = []
arduino_ser = []
stm_ser = []

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

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(32)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = ['/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyACM0']
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def setup_serial_ports():

    logger.debug("************************************")
    logger.debug("     serial port set up start !!    ")
    logger.debug("************************************")

    # detect arduino or stm
    
    comlist = serial_ports()
    temp_arduino_ports = []

    logger.debug(comlist)
    for port in comlist:
        logger.debug(port)

        ser = serial.Serial(port, 115200,timeout=5.0)

        #if port == "/dev/ttyACM0":
        #    stm_ports.append(port)
        #    continue

        line = ser.readline()
        ser.write(b"who:\r\n") 
        logger.debug("[S] who:\r\n") 
        start_time = current_time = time.time()
        search_arduino_ids = False

        while current_time - start_time < 60.0:

            line = ser.readline()
            if len(line) > 0:
                logger.debug("[R] %s" %line)

            if not search_arduino_ids:
                result = re.search(b"arduino",line)
                if result:
                    logger.debug("arduino")
                    ser.write(b"info:,\r\n") 
                    search_arduino_ids = True

            else:
                id0 = ((re.findall(b"id0,[1-9]+",line))[0])[4:]
                id1 = ((re.findall(b"id1,[1-9]+",line))[0])[4:]
                if id0 and id1:
                    logger.debug("port id0 = %s, id1 = %s" %(id0,id1))
                    temp_arduino_ports.append([port,id0,id1])
                    break

            result = re.search(b"stm",line)   
            if result:
                logger.debug("stm")
                stm_ports.append(port)
                break
            
            time.sleep(0.1)
            current_time = time.time()
        
        ser.close()
          
    # motor id check and assign id to detected and sorted port

    
    
    i = 0
    for port in sorted(temp_arduino_ports,key=lambda x:x[1]):
        arduino_ports.append(port[0])
        if port[1].decode('utf-8') in default_motor_id_mapping_6legs.values():
            motor_id_mapping.setdefault(i,port[1].decode('utf-8'))
            id_motor_mapping.setdefault(port[1].decode('utf-8'),i)
            arduino_id_mapping.setdefault(port[1].decode('utf-8'),i)
        else:
            logger.debug("id mismatch happens !!")
            exit()
        if port[2].decode('utf-8') in default_motor_id_mapping_6legs.values():
            motor_id_mapping.setdefault(i+len(temp_arduino_ports),port[2].decode('utf-8'))
            id_motor_mapping.setdefault(port[2].decode('utf-8'),i+len(temp_arduino_ports))
            arduino_id_mapping.setdefault(port[2].decode('utf-8'),i)
        else:
            logger.debug("id mismatch happens !!")
            exit()
        i = i + 1

    logger.debug("arduino_ports = %s" % arduino_ports)
    logger.debug("motor_id_mapping = %s" % motor_id_mapping)
    logger.debug("id_motor_mapping = %s" % id_motor_mapping)
    logger.debug("arduino_id_mapping = %s" % arduino_id_mapping)
    logger.debug("stm_ports = %s" % stm_ports)

    # opening serial ports
    
    if len(arduino_ports) > 0:
        for i in range(len(arduino_ports)):
            for _ in range(5):
                try:
                    s = serial.Serial(arduino_ports[i], 115200,timeout=2.0)
                    break
                except (OSError, serial.SerialException):
                    time.sleep(1.0)
                    pass
            arduino_ser.append(s)
        
    if len(stm_ports) > 0:
        for i in range(len(stm_ports)):
            for _ in range(5):
                try:
                    s = serial.Serial(stm_ports[i], 115200,timeout=2.0)
                    break
                except (OSError, serial.SerialException):
                    time.sleep(1.0)
                    pass
            stm_ser.append(s)

        

    logger.debug("************************************")
    logger.debug("         port set up end   !!       ")
    logger.debug("************************************")



def arduino_command(command,sender_queue):
    if arduino_available == False:
        return

    if command[0] == "move":
        if len(command) == 4:
            item = "legM:id,{0}:xmm,{1}:payload,{2}\r\n".format(motor_id_mapping[command[1]],command[2],command[3])
        elif len(command) == 3:
            item = "legM:id,{0}:xmm,{1}:payload,{2}\r\n".format(motor_id_mapping[command[1]],command[2],motor_height[command[1]])
        sender_queue[arduino_id_mapping[motor_id_mapping[command[1]]]].put(item)
        time.sleep(0.005)
        sender_queue[arduino_id_mapping[motor_id_mapping[command[1]]]].put(item)
        time.sleep(0.005)
        sender_queue[arduino_id_mapping[motor_id_mapping[command[1]]]].put(item)
    else:
        item = "None"
        pass
    
    logger.debug("[S] arduino[%1d]: %s" %(arduino_id_mapping[motor_id_mapping[command[1]]] ,item))  
    time.sleep(0.010)

def stm_command(command,sender_queue):
    if stm_available == False:
        return
        
    print(command)
    if command[0] == "stm_init":
        item = "init\r\n"
        for i in range(legs):
            motor_height[i] = 1
        sender_queue[len(arduino_ports)].put(item)
    elif command[0] == "right":
        if legs == 6:
            #item = "right\r\n"
            item = "aa\r\n"
            for i in range(legs):
                motor_height[i] = i % 2
            sender_queue[len(arduino_ports)].put(item)
        else:
            item = "None"
    elif command[0] == "left":
        if legs == 6:
            #item = "left\r\n"
            item = "bb\r\n"
            for i in range(legs):
                motor_height[i] = (i + 1) % 2
            sender_queue[len(arduino_ports)].put(item)
        else:
            item = "None"
    elif command[0] == "home":
        print(command[0])
        if legs == 6:
            #item = "cc\r\n"
            item = "cc\r\n"
            for i in range(legs):
                motor_height[i] = (i + 1) % 2
            sender_queue[len(arduino_ports)].put(item)
        else:
            item = "None"

    elif command[0] == "up":
        item = "up {}\r\n".format(command[1])
        motor_height[command[1]] = 0
        sender_queue[len(arduino_ports)].put(item)
    elif command[0] == "down":
        item = "down {}\r\n".format(command[1])
        motor_height[command[1]] = 1
        sender_queue[len(arduino_ports)].put(item)
    else:
        item = "None"
    
    if item != "None":
        logger.debug("[S] stm: %s" % item)
        logger.debug("motor_height: %s" % motor_height)
        time.sleep(0.002)
    
    

def sender(queue,ser):
    while True:
        item = queue.get()
        if item is None:
            queue.task_done()
            break
        ser.write(item.encode('utf-8')) 
        while ser.out_waiting > 0:

            time.sleep(0.002)

    
def reader(ser,number):
    while ser.isOpen():
        try:
            line = ser.readline()
            time.sleep(0.001)
        except:
            if number < len(arduino_ports):                
                logger.debug("arduino[%d] exception" %number)                
            else:
                logger.debug("stm port exception")                
            break
        else:
            if len(line) > 0:
                if number < len(arduino_ports):
                    logger.debug("[R] arduino[%d]: %s" %(number,line))
                else:                    
                    logger.debug("[R] stm: %s" % line)
                time.sleep(0.001)

    if number < len(arduino_ports):        
        logger.debug("arduino[%d] port closed" %number)        
    else:
        logger.debug("stm port closed")        

def motion_player(motion,sender_queue):
    logger.debug("motion :: %s" % motion)        
    for command in motion:
        if command[0] == "stm_init":
            stm_command(command,sender_queue)
        elif command[0] == "right":
            stm_command(command,sender_queue)
        elif  command[0] == "left":
            stm_command(command,sender_queue)
        elif  command[0] == "up":
            stm_command(command,sender_queue)
        elif  command[0] == "down":
            stm_command(command,sender_queue)
        elif  command[0] == "home":
            stm_command(command,sender_queue)
        elif  command[0] == "move":
            arduino_command(command,sender_queue)
        elif  command[0] == "wait":
            time.sleep(command[1])
        else:
            pass

def scenario_player(scenario,sender_queue):

    logger.debug("************************************")
    logger.debug("         scenario start !!          ")
    logger.debug("************************************")

    if stm_available and legs == 6:

        for i in range(scenario_repeat):
            logger.debug("---- turn %d / %d ----" % (i+1,scenario_repeat))            
            for motion in scenario:
                motion_player(motion,sender_queue)
    else:
        pass

    logger.debug("************************************")
    logger.debug("         scenario end !!            ")
    logger.debug("************************************")



def menu(sender_queue):

    logger.debug("************************************")
    logger.debug("            start menu              ")
    logger.debug("************************************")
    
    escape_mode = False
    motor_id = -1
    getch = _Getch()
    
    while True:
        key = getch()
        logger.debug('{0} pressed'.format(key))

        if key == b'\x03':
            break

        if key == b'q':
            break

        if escape_mode == False and key in key_command_map:
            command = key_command_map[key]
        elif escape_mode == True and key in linux_esc_key_command_map:
            command = linux_esc_key_command_map[key]
        else:
            continue


        if command[0] == "escape":  
            escape_mode = True

        elif command[0] == "scenario":

            logger.debug("scenario {}".format(command[1]))
            if command[1] == ["walk"]:
                scenario_player(scenario_walk,sender_queue)
            elif command[1] == ["back"]:
                scenario_player(scenario_back,sender_queue)
            else:
                pass
            motor_id = -1
            escape_mode = False

        elif command[0] == "motion":

            logger.debug("motion {}".format(command[1]))
            motion_player(command[1],sender_queue) 
            motor_id = -1
            escape_mode = False

        elif command[0] == "motor_command":

            logger.debug("motor_command {}".format(command[1]))
            if motor_id == -1 :
                logger.debug("motor_id is not set")
                pass

            elif motor_id < 999:
                if command[1] == ["up"]:
                    motor_command = [["up",motor_id]]
                    motion_player(motor_command, sender_queue)
                elif command[1] == ["down"]:
                    motor_command = [["down",motor_id]]
                    motion_player(motor_command, sender_queue)
                elif command[1] == ["move",0]:
                    motor_command = [["move",motor_id,0]]
                    motion_player(motor_command, sender_queue)
                elif command[1] == ["move",100]:
                    motor_command = [["move",motor_id,100]]
                    motion_player(motor_command, sender_queue)
                else:
                    pass

            elif motor_id == 999:
                if command[1] == ["up"]:
                    for i in range(legs):
                        motor_command = [["up",i]]
                        motion_player(motor_command, sender_queue)
                elif command[1] == ["down"]:
                    for i in range(legs):
                        motor_command = [["down",i]]
                        motion_player(motor_command, sender_queue)                        
                elif command[1] == ["move",0]:
                    for i in range(legs):
                        motor_command = [["move",i,0,1]]
                        motion_player(motor_command, sender_queue)
                elif command[1] == ["move",100]:
                    for i in range(legs):
                        motor_command = [["move",i,100,1]]
                        motion_player(motor_command, sender_queue)
                else:
                    pass
            
            escape_mode = False


        elif command[0] == "motor_id":

            motor_id = command[1]
            if motor_id in motor_id_mapping.keys():
                logger.debug("motor_id is set as {}".format(motor_id_mapping[command[1]]))
            elif motor_id == 999:
                logger.debug("motor_id is set as all")
            else:
                logger.debug("motor_id is invalid")
                motor_id = -1
            
            escape_mode = False

        elif command[0] == "command":

            if command[1] == ["clear"]:
                logger.debug("motor_id is cleared")
            elif command[1] == ["stop"]:
                logger.debug("reboot !!")
                os.system("sudo reboot")
            else:
                pass
            motor_id = -1
            escape_mode = False

        else:
            escape_mode = False
 
    
    logger.debug("************************************")
    logger.debug("            end menu                ")
    logger.debug("************************************")


if __name__ == '__main__':

    logger.debug("waiting for 10 sec")
    time.sleep(10.0)

    setup_serial_ports()

    if len(arduino_ports) > 0:
        arduino_available = True
        legs = len(arduino_ports) * 2

    if len(stm_ports) > 0:
        stm_available = True

    if arduino_available == False and stm_available == False:
        logger.debug("No port is available")
        exit()

    
    for i in range(legs):
        motor_height.append(1) # 0:float  1:ground 

    if legs < 6 and legs > 0:

        try:
            key_command_map[b'8'] = ["motor_id",id_motor_mapping['2']]
        except:
            key_command_map[b'8'] = ["motor_id",6]

        try:
            key_command_map[b'9'] = ["motor_id",id_motor_mapping['5']]
        except:
            key_command_map[b'9'] = ["motor_id",6]

        try:
            key_command_map[b'5'] = ["motor_id",id_motor_mapping['3']]
        except:
            key_command_map[b'5'] = ["motor_id",6]

        try:
            key_command_map[b'6'] = ["motor_id",id_motor_mapping['6']]
        except:
            key_command_map[b'6'] = ["motor_id",6]

        try:
            key_command_map[b'2'] = ["motor_id",id_motor_mapping['4']]
        except:
            key_command_map[b'2'] = ["motor_id",6]

        try:
            key_command_map[b'3'] = ["motor_id",id_motor_mapping['7']]
        except:
            key_command_map[b'3'] = ["motor_id",6]


    # start threads

    sender_queue = []
    ts = []
    rs = []

    for i in range(len(arduino_ports)):
        sender_queue.append(queue.Queue())
        ser = arduino_ser[i]
        ser.flush()
        t = threading.Thread(target=sender,args=(sender_queue[i],ser,))
        r = threading.Thread(target=reader,args=(ser,i,))
        t.setDaemon(True)
        r.setDaemon(True)
        ts.append(t)
        rs.append(r)
        t.start()
        r.start()

    if stm_available:
        for i in range(len(stm_ports)):
            sender_queue.append(queue.Queue())
            ser = stm_ser[i]
            ser.flush()
            t = threading.Thread(target=sender,args=(sender_queue[i+ len(arduino_ports)],ser,))
            r = threading.Thread(target=reader,args=(ser,i + len(arduino_ports),))
            t.setDaemon(True)
            r.setDaemon(True)
            ts.append(t)
            rs.append(r)
            t.start()
            r.start()

    time.sleep(2)

    #menu(sender_queue)
    
    scenario_player(scenario_walk,sender_queue)

    logger.debug("closing ports")

    # stop sender queue
    for _ in range(3):
        for q in sender_queue:
            q.put(None)

    # stop serial ports and threads
    for t in ts:
        t.join()

    if len(arduino_ser):
        for ser in arduino_ser:
            ser.close()

    if stm_available:
        if len(stm_ser):
            for ser in stm_ser:
                ser.close()
    
    for r in rs:
        r.join()

    logger.debug("Done!!")

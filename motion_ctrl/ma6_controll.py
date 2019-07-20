import time
import sys
import glob
import serial
import re
import threading
import queue
import os

log_enable = True
stm_available = False
legs = 4
scenario_repeat = 2

if legs == 4:
    motor_id_mapping = {0:"2",1:"3",2:"5",3:"6"}

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

else:
    motor_id_mapping = {0:"2",1:"3",2:"4",3:"5",4:"6",5:"7"}

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

scenario_init = [
    [["stm_init"]],
    [["wait",5.0]]
]

scenario_walk_4legs_ground = [
    [["move",0,0,1],  ["move",2,0,1],
     ["move",1,0,1],  ["move",3,0,1]],

    [["wait",5.0]],
    
    [["move",0,100,1],  ["move",2,100,1],
     ["move",1,100,1],  ["move",3,100,1]],

    [["wait",5.0]]
]

scenario_walk_6legs_ground = [
    [["move",0,0,1],  ["move",3,0,1],
     ["move",1,0,1],  ["move",4,0,1],
     ["move",2,0,1],  ["move",5,0,1]],

    [["wait",5.0]],   
    
    [["move",0,100,1],  ["move",3,100,1],
     ["move",1,100,1],  ["move",4,100,1],
     ["move",2,100,1],  ["move",5,100,1]],

    [["wait",5.0]]
]

scenario_walk_6legs_ground = [
    [["move",0,0,1],  ["move",3,0,1],
     ["move",1,0,1],  ["move",4,0,1],
     ["move",2,0,1],  ["move",5,0,1]],

    [["wait",5.0]],

    [["move",0,100,1],  ["move",3,100,1],
     ["move",1,100,1],  ["move",4,100,1],
     ["move",2,100,1],  ["move",5,100,1]],

    [["wait",5.0]]
]

scenario_walk = [

    [["right"]],

    [["wait",5.0]],

    [["move",0,0,1],  ["move",3,0,1],
     ["move",1,0,1],  ["move",4,0,1],
     ["move",2,0,1],  ["move",5,0,1]],

    [["wait",5.0]],

    [["left"]],

    [["wait",5.0]],

    [["move",0,100,1],  ["move",3,100,1],
     ["move",1,100,1],  ["move",4,100,1],
     ["move",2,100,1],  ["move",5,100,1]],

    [["wait",5.0]]
]



arduino_ports = []
stm_ports = []
arduino_ser = []
stm_ser = []
print_lock = threading.Lock()


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
        ports = glob.glob('/dev/ttyUSB*')
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

def setup():

    comlist = serial_ports()
    temp_arduino_ports = []

    print(comlist)
    for port in comlist:
        print(port)
        ser = serial.Serial(port, 115200,timeout=5.0)
        line = ser.readline()
        ser.write(b"who:\r\n") 
        print("[S] who:\r\n") 
        start_time = current_time = time.time()
        search_arduino_ids = False

        while current_time - start_time < 20.0:

            line = ser.readline()
            if len(line) > 0:
                print("[R] %s" %line)

            if not search_arduino_ids:
                result = re.search(b"arduino",line)
                if result:
                    print("arduino")
                    ser.write(b"info:,\r\n") 
                    search_arduino_ids = True

            else:
                id0 = ((re.findall(b"id0,[1-9]+",line))[0])[4:]
                id1 = ((re.findall(b"id1,[1-9]+",line))[0])[4:]
                if id0 and id1:
                    print("port id0 = %s, id1 = %s" %(id0,id1))
                    temp_arduino_ports.append([port,id0,id1])
                    break

            result = re.search(b"stm",line)   
            if result:
                print("stm")
                stm_ports.append(port)
                break
            
            time.sleep(0.1)
            current_time = time.time()
        
        ser.close()
          
    i = 0
    for port in sorted(temp_arduino_ports,key=lambda x:x[1]):
        arduino_ports.append(port[0])
        arduino_id_mapping.setdefault(port[1].decode('utf-8'),i)
        arduino_id_mapping.setdefault(port[2].decode('utf-8'),i)
        i = i + 1
    print("arduino_ports = %s" % arduino_ports)
    print("arduino_id_mapping = %s" % arduino_id_mapping)
    print("stm_ports = %s" % stm_ports)

    if len(arduino_ports):
        for i in range(len(arduino_ports)):
            for _ in range(5):
                try:
                    s = serial.Serial(arduino_ports[i], 115200,timeout=2.0)
                    break
                except (OSError, serial.SerialException):
                    time.sleep(1.0)
                    pass
#            print(arduino_ports[i])
            arduino_ser.append(s)
        
    if stm_available:
        if len(stm_ports):
            for i in range(len(stm_ports)):
                for _ in range(5):
                    try:
                        s = serial.Serial(stm_ports[i], 115200,timeout=2.0)
                        break
                    except (OSError, serial.SerialException):
                        time.sleep(1.0)
                        pass
    #            print(stm_ports[i])
                stm_ser.append(s)


def arduino_command(command,sender_queue,log_file):
    if command[0] == "move":
        item = "legM:id,{0}:xmm,{1}:payload,{2}\r\n".format(motor_id_mapping[command[1]],command[2],command[3])
        sender_queue[arduino_id_mapping[motor_id_mapping[command[1]]]].put(item)
    else:
        item = "None"
        pass
    
    print_lock.acquire()
    print("[S] arduino[%1d]: %s" %(arduino_id_mapping[motor_id_mapping[command[1]]] ,item))
    if log_enable:
        log_file.write("[S] arduino[%1d]: %s" %(arduino_id_mapping[motor_id_mapping[command[1]]] ,item))
        log_file.write("\n")
    print_lock.release()
    time.sleep(0.005)

def stm_command(command,sender_queue,log_file):
    if command[0] == "stm_init":
        item = "init\r\n"
        sender_queue[3].put(item)
    elif command[0] == "right":
        item = "right\r\n"
        sender_queue[3].put(item)
    elif command[0] == "left":
        item = "left\r\n"
        sender_queue[3].put(item)
    else:
        item = "None"
        pass
    
    print_lock.acquire()
    print("[S] stm: %s" % item)
    if log_enable:
        log_file.write("[S] stm: %s" % item)
        log_file.write("\n")
    print_lock.release()
    time.sleep(0.002)

def sender(queue,ser,log_file):
#    print("sender start")
    while True:
        item = queue.get()
        if item is None:
            queue.task_done()
            break
        ser.write(item.encode('utf-8')) 
#        print(item.encode('utf-8'))
        while ser.out_waiting > 0:

            time.sleep(0.002)

    
def reader(ser,number,log_file):
#    print("reader start")
    while ser.isOpen():
        try:
            line = ser.readline()
            time.sleep(0.001)
        except:
            if number < len(arduino_ports):
                print_lock.acquire()
                print("arduino[%d] exception" %number)
                if log_enable:
                    log_file.write("arduino[%d] exception" %number)
                    log_file.write("\n")
                print_lock.release()
            else:
                print_lock.acquire()
                print("stm port exception")
                if log_enable:
                    log_file.write("stm port exception")
                    log_file.write("\n")
                print_lock.release()
            break
        else:
            if len(line) > 0:
                if number < len(arduino_ports):
                    print_lock.acquire()
                    print("[R] arduino[%d]: %s" %(number,line))
                    if log_enable:
                        log_file.write("[R] arduino[%d]: %s" %(number,line))
                        log_file.write("\n")
                    print_lock.release()
                else:
                    print_lock.acquire()
                    print("[R] stm: %s" % line)
                    if log_enable:
                        log_file.write("[R] stm: %s" % line)
                        log_file.write("\n")
                    print_lock.release()
                time.sleep(0.001)

    if number < len(arduino_ports):
        print_lock.acquire()
        print("arduino[%d] port closed" %number)
        if log_enable:
            log_file.write("arduino[%d] port closed" %number)
            log_file.write("\n")
        print_lock.release()
    else:
        print_lock.acquire()
        print("stm port closed")
        if log_enable:
            log_file.write("stm port closed")
            log_file.write("\n")
        print_lock.release()

def player(scenario,sender_queue,log_file):
    for motion in scenario:
        print_lock.acquire()
        print("motion :: %s" % motion) 
        if log_enable:
            log_file.write("motion :: %s" % motion)
            log_file.write("\n")
        print_lock.release()
        for command in motion:
            if command[0] == "stm_init":
                stm_command(command,sender_queue,log_file)
            elif command[0] == "right":
                stm_command(command,sender_queue,log_file)
            elif  command[0] == "left":
                stm_command(command,sender_queue,log_file)
            elif  command[0] == "move":
                arduino_command(command,sender_queue,log_file)
            elif  command[0] == "wait":
                time.sleep(command[1])
            else:
                pass

def main():

    print("************************************")
    print("         port set up start !!       ")
    print("************************************")

    # start serial ports

    setup()

    if len(arduino_ports) != legs/2:
        print("Error Number of legs and aruduino is mismatched !!")
        exit()
    
    if len(stm_ports) == 0 and stm_available == True:
        print("Error stm is expected to be available !!")
        exit()

    # log file open
    if log_enable:
        os.makedirs("./log",exist_ok=True)
        file_name = "./log/log-" + time.strftime("%Y%m%d-%H%M%S", time.strptime(time.ctime()))+".txt"
        log_file = open(file_name,'w')
    else:
        log_file = None

    # start threads

    sender_queue = []
    ts = []
    rs = []

    for i in range(len(arduino_ports)):
        sender_queue.append(queue.Queue())
        ser = arduino_ser[i]
        ser.flush()
        t = threading.Thread(target=sender,args=(sender_queue[i],ser,log_file,))
        r = threading.Thread(target=reader,args=(ser,i,log_file,))
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
            t = threading.Thread(target=sender,args=(sender_queue[i+ len(arduino_ports)],ser,log_file,))
            r = threading.Thread(target=reader,args=(ser,i + len(arduino_ports) ,log_file,))
            t.setDaemon(True)
            r.setDaemon(True)
            ts.append(t)
            rs.append(r)
            t.start()
            r.start()
    
    print("************************************")
    print("         port set up end   !!       ")
    print("************************************")

    time.sleep(2)

    print("************************************")
    print("         scenario start !!          ")
    print("************************************")

    if stm_available:
        print_lock.acquire()
        print("---- init ----")
        if log_enable:
            log_file.write("---- init ----")
            log_file.write("\n")
        print_lock.release()
        player(scenario_init,sender_queue,log_file)

    for i in range(scenario_repeat):
        print_lock.acquire()
        print("---- turn %d / %d ----" % (i+1,scenario_repeat))
        if log_enable:
            log_file.write("---- turn %d / %d ----" % (i+1,scenario_repeat))
            log_file.write("\n")
        print_lock.release()
        if stm_available and legs == 6:
            player(scenario_walk,sender_queue,log_file)
        elif legs == 4:
            player(scenario_walk_4legs_ground,sender_queue,log_file)
        elif legs == 6:
            player(scenario_walk_6legs_ground,sender_queue,log_file)

    print("************************************")
    print("         scenario end !!            ")
    print("************************************")
    
    print("closing ports")

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

    # log file close
    if log_enable:
        log_file.close()


    print("Done!!")
        

if __name__ == '__main__':
    main()

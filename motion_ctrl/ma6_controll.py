import time
import sys
import glob
import serial
import re
import threading
import queue

motor_id_mapping = {0:"2",1:"3",2:"4",3:"5",4:"6",5:"7"}
arduino_id_mapping = {}

scenario = [
            [["stm_init"]],
            [["wait",3.0]],
            [["right"]],
            [["forward",0],["back",1],["forward",2],["back",3],["forward",4],["back",5]],
            [["left"]],
            [["back",0],["forward",1],["back",2],["forward",3],["back",4],["forward",5]]
           ]

scenario_repeat = 5
scenario_wait = 2.0
motion_interval = 2.0

arduino_ports = []
stm_ports = []
arduino_ser = []
stm_ser = []

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
        ser = serial.Serial(port, 115200,timeout=2)
        line = ser.readline()
        ser.write(b"who:\r\n") 
        start_time = current_time = time.time()
        search_arduino_ids = False

        while current_time - start_time < 5.0:

            line = ser.readline()

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
        
#    print(temp_arduino_ports)
#    print(sorted(temp_arduino_ports,key=lambda x:x[1]))
    
   
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
                    s = serial.Serial(arduino_ports[i], 115200)
                    break
                except (OSError, serial.SerialException):
                    time.sleep(1.0)
                    pass
#            print(arduino_ports[i])
            arduino_ser.append(s)
        
    if len(stm_ports):
        for i in range(len(stm_ports)):
            for _ in range(5):
                try:
                    s = serial.Serial(stm_ports[i], 115200)
                    break
                except (OSError, serial.SerialException):
                    time.sleep(1.0)
                    pass
#            print(stm_ports[i])
            stm_ser.append(s)


def arduino_command(command,sender_queue):
    if command[0] == "forward":
        item = "legM:id,{0}:xmm,0\r\n".format(motor_id_mapping[command[1]])
        print("[S] arduino[%1d]: %s" %(arduino_id_mapping[motor_id_mapping[command[1]]] ,item))
        sender_queue[arduino_id_mapping[motor_id_mapping[command[1]]]].put(item)
    elif  command[0] == "back":
        item = "legM:id,{0}:xmm,150\r\n".format(motor_id_mapping[command[1]])
        print("[S] arduino[%1d]: %s" %(arduino_id_mapping[motor_id_mapping[command[1]]],item))
        sender_queue[arduino_id_mapping[motor_id_mapping[command[1]]]].put(item)
    else:
        pass

def stm_command(command,sender_queue):
    if command[0] == "stm_init":
        item = "init\r\n"
        print("[S] stm: %s" % item)
        sender_queue[3].put(item)
    elif command[0] == "right":
        item = "right\r\n"
        print("[S] stm: %s" % item)
        sender_queue[3].put(item)
    elif command[0] == "left":
        item = "left\r\n"
        print("[S] stm: %s" % item)
        sender_queue[3].put(item)
    else:
        pass

def sender(queue,ser):
#    print("sender start")
    while True:
        item = queue.get()
        if item is None:
            queue.task_done()
            break
        ser.write(item.encode('utf-8')) 
#        print(item.encode('utf-8'))
        while ser.out_waiting > 0:
            time.sleep(0.001)

    
def reader(ser,number):
#    print("reader start")
    while ser.isOpen():
        try:
            line = ser.readline()
        except:
            break
        else:
            if line is not None:
               #ser.flushInput()
               if number < len(arduino_ports):
                   print("[R] arduino[%d]: %s" %(number,line))
               else:
                   print("[R] stm: %s" % line)
               time.sleep(0.005)

    if number < len(arduino_ports):
        print("arduino[%d] port closed" %number)
    else:
        print("stm port closed")


def player(scenario,sender_queue):
    for motion in scenario:
        print("motion :: %s" % motion) 
        for command in motion:
            if command[0] == "stm_init":
                stm_command(command,sender_queue)
            elif command[0] == "right":
                stm_command(command,sender_queue)
            elif  command[0] == "left":
                stm_command(command,sender_queue)
            elif  command[0] == "forward":
                arduino_command(command,sender_queue)
            elif  command[0] == "back":
                arduino_command(command,sender_queue)
            elif  command[0] == "wait":
                time.sleep(command[1])
            else:
                pass
        time.sleep(motion_interval)

def main():

    print("************************************")
    print("         port set up start !!       ")
    print("************************************")

    # start serial ports

    setup()

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

    for i in range(len(stm_ports)):
        sender_queue.append(queue.Queue())
        ser = stm_ser[i]
        ser.flush()
        t = threading.Thread(target=sender,args=(sender_queue[i+ len(arduino_ports)],ser,))
        r = threading.Thread(target=reader,args=(ser,i + len(arduino_ports) ,))
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

    for i in range(scenario_repeat):
        print("---- turn %d / %d ----" % (i+1,scenario_repeat))
        player(scenario,sender_queue)
        print("wait %d sec" %scenario_wait)
        time.sleep(scenario_wait)

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
    #print("sender done")
    time.sleep(1.0)

    if len(arduino_ser):
        for ser in arduino_ser:
            ser.close()
            time.sleep(3.0)
    
    #print("arduino ser closed")

    if len(stm_ser):
        for ser in stm_ser:
            ser.close()
            time.sleep(3.0)

    #print("stm ser closed")
    
    #print("len(rs) %d" %(len(rs)))
    for r in rs:
        r.join()
    
    #print("reader closed")
    
    print("Done!!")
        

if __name__ == '__main__':
    main()

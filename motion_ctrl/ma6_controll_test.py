import time
import sys
import glob
import serial
import re
import threading
import queue

#arduino_id_mapping = {"2":0,"3":1,"4":2,"5":0,"6":1,"7":2}
arduino_id_mapping = {"4":0,"7":0}


scenario = [
#           [["right"]],
#            [["forward","2"]],
#            [["forward","3"]],
            [["forward","4"]],
#            [["forward","5"]],
#            [["forward","6"]],
            [["forward","7"]],
#            [["left"]],
#            [["back","2"]],
#            [["back","3"]],
            [["back","4"]],
#            [["back","5"]],
#            [["back","6"]],
            [["back","7"]]
           ]


scenario_repeat = 5
scenario_wait = 2
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
        ports = glob.glob('/dev/tty[A-Za-z]*')
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
        line = ser.readline()
        print(line)
        line = ser.readline()
        #print(line)
        #line = ser.readline()
        #print(line)
        result = re.search(b"arduino",line)
        if result:
            print("arduino")
            ser.write(b"info:,\r\n") 
            info = ser.readline()
            print(info)
            id0 = ((re.findall(b"id0,[1-9]+",info))[0])[4:]
            id1 = ((re.findall(b"id1,[1-9]+",info))[0])[4:]
            temp_arduino_ports.append([port,id0,id1])
            
        else:
            result = re.search(b"stm",line)
            if result:
                print("stm")
                stm_ports.append(port)
        ser.close()
        
#    print(temp_arduino_ports)
#    print(sorted(temp_arduino_ports,key=lambda x:x[1]))
    for port in sorted(temp_arduino_ports,key=lambda x:x[1]):
        arduino_ports.append(port[0])
    print(arduino_ports)
#   print(stm_ports)

    for i in range(len(arduino_ports)):
        print(arduino_ports[i])
        arduino_ser.append(serial.Serial(arduino_ports[i], 115200))
    
#    for i in range(len(stm_ports)):
#        print(stm_ports[i])
#        stm_ser.append(serial.Serial(stm_ports[i], 115200))


def arduino_command(command,sender_queue):
    if command[0] == "forward":
        item = "legM:id,{0}:xmm,0\r\n".format(command[1])
        print("[S] arduino[%1d]: %s" %(arduino_id_mapping[command[1]],item))
        sender_queue[arduino_id_mapping[command[1]]].put(item)
    elif  command[0] == "back":
        item = "legM:id,{0}:xmm,150\r\n".format(command[1])
        print("[S] arduino[%1d]: %s" %(arduino_id_mapping[command[1]],item))
        sender_queue[arduino_id_mapping[command[1]]].put(item)
    else:
        pass

def stm_command(command,sender_queue):
    if command[0] == "stm_init":
        item = "init\r\n"
        print("[S] stm: %s" % item)
        sender_queue[3].put(item)
    elif command[0] == "height":
        item = "height:{0}\r\n".format(command[1])
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
    print("sender start")
    while True:
        item = queue.get()
        if item is None:
            break
        ser.write(item.encode('utf-8')) 
#        print(item.encode('utf-8'))
        while ser.out_waiting > 0:
            time.sleep(0.001)
        queue.task_done()
    
def reader(ser,number):
    print("reader start")
    while ser.isOpen():
        line = ser.readline()
        if line is not None:
            #ser.flushInput()
            if number < len(arduino_ports):
                print("[R] arduino[%d]: %s" %(number,line))
            else:
                print("[R] stm: %s" % line)
            time.sleep(0.005)
    print("serial closed")

def player(scenario,sender_queue):
    for motion in scenario:
        print("motion :: %s" % motion) 
        for command in motion:
            if command[0] == "right":
                stm_command(command,sender_queue)
            elif  command[0] == "left":
                stm_command(command,sender_queue)
            elif  command[0] == "forward":
                arduino_command(command,sender_queue)
            elif  command[0] == "back":
                arduino_command(command,sender_queue)
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

#    for i in range(len(stm_ports)):
#        sender_queue.append(queue.Queue())
#        ser = stm_ser[i]
#        ser.flush()
#        t = threading.Thread(target=sender,args=(sender_queue[i],ser,))
#        r = threading.Thread(target=reader,args=(ser,i + len(arduino_ports),))
#        t.setDaemon(True)
#        r.setDaemon(True)
#        ts.append(t)
#        rs.append(r)
#        t.start()
#        r.start()

    print("************************************")
    print("         port set up end   !!       ")
    print("************************************")

    time.sleep(2)

    print("************************************")
    print("         scenario start !!          ")
    print("************************************")

    for i in range(scenario_repeat):
        print("---- turn %1d ----" % (i+1))
        player(scenario,sender_queue)
        time.sleep(scenario_wait)

    print("************************************")
    print("         scenario end !!            ")
    print("************************************")

    # stop sender queue
       
    sender_queue[0].put(None)
#    sender_queue[1].put(None)
#    sender_queue[2].put(None)    
#    sender_queue[3].put(None)

    sender_queue[0].put(None)
#    sender_queue[1].put(None)
#    sender_queue[2].put(None)    
#    sender_queue[3].put(None)

    sender_queue[0].put(None)
#    sender_queue[1].put(None)
#    sender_queue[2].put(None)    
#    sender_queue[3].put(None)

    # stop serial ports and threads

    for t in ts:
        t.join()

    for ser in arduino_ser:
        ser.close()

#    for ser in stm_ser:
#        ser.close()

    for r in rs:
        r.join()
        

if __name__ == '__main__':
    main()

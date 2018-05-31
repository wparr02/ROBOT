import time
#import enaml

from serial import Serial

s = Serial('/dev/ttyACM0', 9600)  # open serial port
ACK = 0

while True:
    #x = input()
    x = "-75.00/10.00/10.00/10.00/10.00"

    s.write(bytes(x, 'UTF-8'))
    print(str(s.readline())[2:-5])
    time.sleep(5)

    #x = "-50.00/-10.00/-10.00/-10.00/-10.00"

    #s.write(bytes(x, 'UTF-8'))
    #print(str(s.readline())[2:-5])
    #time.sleep(3)

 

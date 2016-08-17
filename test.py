from time import sleep
import serial
import select
import sys

from sys import argv
script, dfile = argv

from datetime import datetime
import io
dt = str(datetime.now())

outF = open(dfile, 'a', 0) #append mode, no buffer flush immideately
data = dt + "\n"
outF.write(data)
outF.close()

ser = serial.Serial('/dev/ttyUSB0', 115200)

while True:
    outF = open(dfile,'a')
    #counter += 1
    #ser.write('<') # this is required for 160815mon1655_startend
    #ser.write(str(chr(counter))) # convert decimal # to ASCII then send to ard
    #ser.write('>')
    #print ser.readline()
    #sleep(.1)
    
    # check whether the user has typed anything (.2 sec timeout)
    inp, outp, err = select.select([sys.stdin, ser], [], [], .2)

    # if the user typed anything, send
    if sys.stdin in inp :
        line = sys.stdin.readline()
        ser.write(line)

    # if the arduino has printed anything, display
    if ser in inp :
        line = ser.readline().strip()
        outF.write(line + "\n")
        outF.close()
        print "Arduino: ", line

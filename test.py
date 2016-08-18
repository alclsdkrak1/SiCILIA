from time import sleep
import time
import serial
import select
import sys

from sys import argv
script, dfile = argv

from datetime import datetime
import io
dt = str(datetime.now()) #time for recording
t0 = time.time() #to calculate elapsed time
tT = 10 # 60 sec period for sampling rate/threshold -> 10 sec for testing

outF = open(dfile, 'a', 0) #append mode, no buffer flush immideately
data = dt + "\n\n"
outF.write(data)
outF.close() # do not close yet, instead use "with-try" to access file

isReady = False

#ser = serial.Serial('/dev/ttyUSB0', 115200) #for nano
ser = serial.Serial('/dev/ttyACM0', 115200) #for uno


while True:
    outF = open(dfile,'a', 0) # only need to be opened once with if not buffering
    #counter += 1
    #ser.write('<') # this is required for 160815mon1655_startend
    #ser.write(str(chr(counter))) # convert decimal # to ASCII then send to ard
    #ser.write('>')
    #print ser.readline()
    #sleep(.1)
    
    # check whether the user has typed anything (.2 sec timeout)
    inp, outp, err = select.select([sys.stdin, ser], [], [], .2)

    t1 = time.time()
    tE = t1 - t0 # elapsed time
    #t0 = t1 # refresh time once data collected

    # if the user typed anything, send
    if sys.stdin in inp :
        line = sys.stdin.readline()
        ser.write(line)

    # if the arduino has printed anything, display
    if ser in inp :
        print 'elapsed time is ', tE
        line = ser.readline().strip()
        #outF.write(line + "\n")
        if (line == '1'): # arduino sends OK to collect data
            isReady = True
            print 'arduino is ready '
        else: # new documenting block, when arduino sends something
            if (line == 'Take measurement'):
                outF.write('\n')
            outF.write(str(datetime.now()) + ' & ')
            outF.write(line + "\n")
        if (tE >= tT) and (isReady == True):
#            outF.write(str(datetime.now() + '\n') # data collection time
            t0 = t1 # refresh time once data collected
            isReady = False            
            ser.write('<1>\r')
            sleep(.1)
# For some reason bottom block does not work,
# other than print. put these way up.
#        if (line != '1'): # arduino sends any other message, document it
#            outF.write(str(datetime.now()))
#            outF.write('\n')
#            outF.write(line)
#            outF.write('\n')
#            print 'arduino : ', line
    outF.close()

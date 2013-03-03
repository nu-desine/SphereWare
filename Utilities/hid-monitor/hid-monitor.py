import hid
import struct
import sys
import subprocess 
import os
import datetime
import time
import bitarray
import pprint
from pylab import *

try:
    look_at_pad = sys.argv[1].split(":")
    if len(look_at_pad) > 1:
        look_at_pad = range(int(look_at_pad[0]), int(look_at_pad[1]) + 1)
        print "logging only pads in range: ", look_at_pad[0], " to ", look_at_pad[-1]
    else:
        look_at_pad = [int(item) for item in sys.argv[1].split(",")]
        if look_at_pad > 1:
            print "logging pads: ", look_at_pad
        else:
            print "logging only pad: ", look_at_pad
except IndexError:
    look_at_pad = range(0, 48)
    print "logging all pads"

print look_at_pad
print "Opening device"
h = hid.device(0x1d50, 0x6021)
h.set_nonblocking(0)


#now = datetime.datetime.now()

#log_name = "%d.%d.%d_%d:%d." % (now.year, now.month, now.day, now.hour, now.minute)
#print "please add a comment:",
#log_name += raw_input() + ".log"
#print log_name
#
#f = open(log_name, 'w')
#
#for d in hid.enumerate(0, 0):
#      keys = d.keys()
#    keys.sort()
#    for key in keys:
#        print "%s : %s" % (key, d[key])
#    print ""
#

#fixed decimal to fixed digit binary
def bin(x, digits=0):
    oct2bin = ['000','001','010','011','100','101','110','111']
    binstring = [oct2bin[int(n)] for n in oct(x)]
    return ''.join(binstring).lstrip('0').zfill(digits) 

def tobits(s):
    result = []
    for c in s:
        bits = bin(ord(c))[2:]
        bits = '00000000'[len(bits):] + bits
        result.extend([int(b) for b in bits])
    return result

def frombits(bits):
    chars = []
    for b in range(len(bits) / 10):
        chunk = bits[b*10:(b+1)*10 - 1]
        print int("".join([str(a) for a in bits]), 2)


data = {} 
velocities = [0] * 48
prev_report = 0
prev_buttons_and_dials = 0;

triggered = []
try:
    while 1:
        report = h.read(104)
        #print len(report)
        if report[0] == 0x01:
            for index, first_byte in enumerate(report[1:-1:2]):
                second_byte = report[index * 2 + 2]
                velocity = first_byte & 0x7F
                pressure = (first_byte >> 7) | (second_byte << 1)
                data[index] =  (pressure, velocity)
                #if velocity > 0:
                    #print index, pressure, velocity
                    #triggered.append((pad,velocity))
            if (report[97] != 0b1):
                print "buttons: ",
                print bin(report[97], 8);
            if (report[98] != 0):
                print "dial 1: ",
                if (report[98] > 127):
                    print -(256 - report[98])
                else:
                    print report[98]
            if (report[99] != 0):
                print "dial 2: ",
                if (report[99] > 127):
                    print -(256 - report[99])
                else:
                    print report[99]
            #for index in data:
            #    if (dataindex[1] > 0):
            #        sys.stdout.write("(%2i," % index)
            #        sys.stdout.write("%3i," % data[index][0])
            #        sys.stdout.write("%3i)" % data[index][1])
            #        sys.stdout.write("     ")
                #if count > 5:
                #    sys.stdout.write("\r\n")
                #    count = 0
            #sys.stdout.write("\r\n")
        elif report[0] == 0x02:
            for index, first_byte in enumerate(report[1:-1:2]):
                if index in look_at_pad:
                    second_byte = report[index * 2 + 2]
                    #value = first_byte | (second_byte << 8)
                    value = struct.unpack("h", "".join(map(chr, [first_byte, second_byte])))[0] 
                    data[index] = value
            count = 0
            for index in data:
                count += 1
                sys.stdout.write("(%2i," % index)
                sys.stdout.write("%5i)" % data[index])
                sys.stdout.write("     ")
                #if count > 5:
                #    sys.stdout.write("\r\n")
                #    count = 0
                    
            sys.stdout.write("\r\n")



        #    print report[1:3]
        #    data = []
        #    for byte in report[1:2]:
        #        l = [_ for _ in bin(byte, 8)]
        #        l.reverse()
        #        for b in l:
        #            data.append(b)
        #    print data
        #    #print struct.unpack("h", "".join(map(chr, report[10:12])))[0] 
        #    #data = []
        #    #for byte in report[1:62]:
        #    #    for s in bin(byte,8):
        #    #        data.append(s)
        #    #print data[0]

except KeyboardInterrupt:
    print ""
    h.close();
    #f.close()
except:
    #subprocess.call(["mplayer", "ALARM.WAV"])
    #f.close()
    h.close();
    raise

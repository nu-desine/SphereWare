import hid
import struct
import sys
import subprocess 
import os
import datetime
import time

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
h.set_nonblocking(1)

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


data = [] 
try:
    #counter = 0;
    while 1:
        report = h.read(5)
        try:
            if report[0] == 0x01:
                    decoded = (report[1], struct.unpack("h", "".join(map(chr, report[2:4])))[0], report[4])
                    if report[1] in look_at_pad:
                        data.append(decoded[1])
                        print decoded
        except IndexError:
            time.sleep(0.001)
            pass
        #f.write(str(decoded) + '\n')
        #buf = [ 0x00, 0x06, 0x90, (counter & 0xFF), 0x00] 
        #h.write(buf)
        #buf = [ 0x00, 0x06, 0x80, (counter & 0xFF), 0x00] 
        #h.write(buf)
        #counter += 1
        #if (counter > 127):
        #    counter = 0;
except KeyboardInterrupt:
    print ""
    h.close();
    #f.close()
except:
    #subprocess.call(["mplayer", "ALARM.WAV"])
    #f.close()
    h.close();
    raise

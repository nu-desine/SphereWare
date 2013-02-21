import hid
import struct
import sys

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


data = [] 
velocities = [0] * 48
try:
    #counter = 0;
    while 1:
        report = h.read(5)
        print report
        #if report[0] == 0x01:
        #    for i in range(report[1]):
        #        decoded = (report[(i*4)+2], struct.unpack("h", "".join(map(chr, report[(i*4) + 3:(i*4) + 5])))[0], report[(i*4) + 5]) 
        #        if decoded[0] in look_at_pad:
        #            print decoded
        ###try:
        #if report[0] == 0x01:
        #    decoded = (report[1], struct.unpack("h", "".join(map(chr, report[2:4])))[0], report[4]) 
        #    if report[1] in look_at_pad:
        #        print decoded

        #for i in range(1, 145, 3):
        #    pad = i / 3
        #    value = struct.unpack("h", "".join(map(chr, report[i:i+2])))[0]
        #    velocity = report[i+2]
        #    velocities[pad] = (velocity, pad)

        #    print pad, value, velocity

            #if (velocity != 0):
            #    velocities[pad] = report[i+2]
            #print pad, value, velocities[pad]
                #decoded = (report[1], struct.unpack("h", "".join(map(chr, report[i + 2: i + 4])))[0], report[i + 3]) 


            #for char in report[1:256]:
            #    if char !=0:
            #        print char

            #if report[1] in look_at_pad:
            #    #if decoded[2] != 0:
            #    data.append(decoded[1])
            #    print decoded
                #print bin(decoded[2], 8), decoded
        #if report[0] == 0x04:
        #    for i in range(2, len(report)-1, 2):
        #        #print report[i:i+2]
        #        decoded = struct.unpack("h", "".join(map(chr, report[i:i+2])))[0]
        #        data.append(-decoded)
        #    print average(data)
        #    #plot(data)
        #    #index = data.index(min(data))
        #    #velo = min(data)
        #    #if index > 80:
        #    #    print -velo
        #    data = []


        #except IndexError:
        #    time.sleep(0.001)
        #    pass
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

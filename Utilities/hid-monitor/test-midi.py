import hid
import time
import subprocess

while 1:
    print "Press [Enter] to run."
    raw_input();
    h = hid.device(0x1d50, 0x6021)
    h.set_nonblocking(0)
    if h:
        l = [0x00] * 128
        l[0] = 0x01
        l[1] = 0x01
        l[2] = 0x00
        l[3] = 144
        l[4] = 60
        l[5] = 69
        h.write(l);

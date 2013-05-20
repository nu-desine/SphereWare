import hid
import time
import subprocess

while 1:
    print "Press [Enter] to run."
    raw_input();
    subprocess.call(["make", "first"])
    time.sleep(1.2);
    h = hid.device(0x1d50, 0x6021)
    h.set_nonblocking(0)
    if h:
        l = [0x02] * 256
        h.write(l);
    report = [0] * 256
    print "Waiting for main-board button-press..."
    while (report[243] != 0xAA):
        report = h.read(256)
    report = h.read(256)
    if report[245] < 48:
        print "FAILURE on analog input ", int(report[245]) + 1
    elif report[245] == 66:
        print "FAILURE on USB connection"
    elif report[245] == 77:
        print "FAILURE on R2R DAC output"

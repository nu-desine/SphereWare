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
        l = [0x02] * 128
        h.write(l);
    report = [0] * 128
    print "Waiting for main-board button-press..."
    while (report[97] != 100):
        report = h.read(128)

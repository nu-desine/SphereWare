import hid
import time
import curses
import struct

data = [(0,0)] * 48 

translate = [
"1A - Gray   (1)",
"1A - Purple (2)",
"1A - Blue   (3)",
"1A - Green  (4)",
"1B - Gray   (1)",
"1B - Purple (2)",
"1B - Blue   (3)",
"1B - Green  (4)",
"2A - Gray   (1)",
"2A - Purple (2)",
"2A - Blue   (3)",
"2A - Green  (4)",
"2B - Gray   (1)",
"2B - Purple (2)",
"2B - Blue   (3)",
"2B - Green  (4)",
"3A - Gray   (1)",
"3A - Purple (2)",
"3A - Blue   (3)",
"3A - Green  (4)",
"3B - Gray   (1)",
"3B - Purple (2)",
"3B - Blue   (3)",
"3B - Green  (4)",
"4A - Gray   (1)",
"4A - Purple (2)",
"4A - Blue   (3)",
"4A - Green  (4)",
"4B - Gray   (1)",
"4B - Purple (2)",
"4B - Blue   (3)",
"4B - Green  (4)",
"5A - Gray   (1)",
"5A - Purple (2)",
"5A - Blue   (3)",
"5A - Green  (4)",
"5B - Gray   (1)",
"5B - Purple (2)",
"5B - Blue   (3)",
"5B - Green  (4)",
"6A - Gray   (1)",
"6A - Purple (2)",
"6A - Blue   (3)",
"6A - Green  (4)",
"6B - Gray   (1)",
"6B - Purple (2)",
"6B - Blue   (3)",
"6B - Green  (4)",
]

stdscr = curses.initscr()
curses.noecho()
#curses.curs_set(0)
begin_x = 0 ; begin_y = 0
height = 50 ; width = 40
win = curses.newwin(height, width, begin_y, begin_x)

while 1:
    try:
        h = hid.device(0x1d50, 0x6021)
        h.set_nonblocking(0)
        while 1:
            report = h.read(101)
            #print len(report)
            if report[0] == 0x01:
                for index, first_byte in enumerate(report[1:97:2]):
                    second_byte = report[index * 2 + 2]
                    passed = first_byte & 0x01
                    tested = (first_byte >> 7) & 0x01
                    data[index] = (tested, passed)
                if data[0][0]:
                    win.addstr(0, 0, "Testing rows 1,2 & 3.", 0)
                    win.clrtoeol()
                    #win.refresh()
                elif data[24][0]:
                    win.addstr(0, 0, "Testing rows 4,5 & 6.", 0)
                    win.clrtoeol()
                    #win.refresh()

                failures = []
                for index,val in enumerate(data):
                    if (val[0] and (not val[1])):
                            failures.append(index)
                if failures != []:
                    win.addstr(1,0, "FAILED:", 0)
                    #win.refresh()
                    for i,f in enumerate(failures):
                        win.addstr(i+2, 0, translate[f], 0)
                        win.clrtoeol()

                win.clrtobot()
                win.refresh()
            if report[0] == 0x02:
                for index, first_byte in enumerate(report[1:-1:2]):
                     second_byte = report[index * 2 + 2]
                     #value = first_byte | (second_byte << 8)
                     value = struct.unpack("h", "".join(map(chr, [first_byte, second_byte])))[0] 
                     win.addstr(index,0, str((index,value)), 0)
                     win.clrtoeol()
                win.refresh()
               



                     

    except (IndexError, IOError):
        try:
            win.addstr(0, 0, "Nothing connected via USB...", 0)
            win.clrtobot()
            win.refresh()
            #subprocess.call(["mplayer", "ALARM.WAV"])
            #f.close()
            #h.close();
            time.sleep(1)
        except:
            h.close();
            curses.echo()
            curses.endwin()
            raise
    except:
        h.close();
        curses.echo()
        curses.endwin()
        raise
        #f.close()

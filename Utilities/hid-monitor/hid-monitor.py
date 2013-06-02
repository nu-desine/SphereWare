import hid
import time
import curses
import struct

data = [(0,0)] * 48 

translate = [
"1A - Gray",
"1A - Purple",
"1A - Blue",
"1A - Green",
"1B - Gray",
"1B - Purple",
"1B - Blue",
"1B - Green",
"2A - Gray",
"2A - Purple",
"2A - Blue",
"2A - Green",
"2B - Gray",
"2B - Purple",
"2B - Blue",
"2B - Green",
"3A - Gray",
"3A - Purple",
"3A - Blue",
"3A - Green",
"3B - Gray",
"3B - Purple",
"3B - Blue",
"3B - Green",
"4A - Gray",
"4A - Purple",
"4A - Blue",
"4A - Green",
"4B - Gray",
"4B - Purple",
"4B - Blue",
"4B - Green",
"5A - Gray",
"5A - Purple",
"5A - Blue",
"5A - Green",
"5B - Gray",
"5B - Purple",
"5B - Blue",
"5B - Green",
"6A - Gray",
"6A - Purple",
"6A - Blue",
"6A - Green",
"6B - Gray",
"6B - Purple",
"6B - Blue",
"6B - Green"
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

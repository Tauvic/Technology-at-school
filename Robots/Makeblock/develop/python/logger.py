import msvcrt
import time, sys
import string
import numpy
from pywinusb import hid
# controling mBot thru serial wifi with numerical keys 2,4,5,6,8
# https://github.com/vrbaj/mbot

# handler called when a report is received
def rx_handler(data):
    print data
    for x in data:
      if (x == ord('@')):  
          sys.stdout.write('\n')
      if (x >= ord(' ')):
          sys.stdout.write(chr(x))
    sys.stdout.flush()
    

#def main(verbose=True):
if __name__ == '__main__':    
    
    filter1 = hid.HidDeviceFilter(vendor_id=0x0416, product_id=0xffff)  # find serial dongle
    devices = filter1.get_devices()

    if devices:
        device = devices[0]
        print("wifi serial line dongle connected")
    else:
        print("no wifi found")
        sys.exit()


    print(device)
    print("Open")
    device.open()
    device.set_raw_data_handler(rx_handler)

    print("Type Q to quit..")


    color = 0xFF

                    
    while True:
        try:
            if msvcrt.kbhit():
                key = msvcrt.getch()
                key_number = ord(key)
                #print(key_number)  # stroked key number
                if key_number == 113:
                    break
                if key_number == 72:  # go ahead 0xff, 0x55, 0x09, 0x00, 0x02, 0x08, 0x07, 0x02, 0x00, r, g, b
                    print("Send data")
                    color = ~color
                    #device.open()
                    out_report = device.find_output_reports()[0]
                    my_buffer = [0x0] * 31
                    my_buffer[0]  = 0x0
                    my_buffer[1]  = 12
                    my_buffer[2]  = 0xff
                    my_buffer[3]  = 0x55
                    my_buffer[4]  = 0x09
                    my_buffer[5]  = 0x00
                    my_buffer[6]  = 0x02 #RUN
                    my_buffer[7]  = 0x08 #device =8
                    my_buffer[8]  = 0x07 #port
                    my_buffer[9]  = 0x02 #slot
                    my_buffer[10]  = 0x01 #led
                    my_buffer[11] = color #R
                    my_buffer[12] = 0x00 #G
                    my_buffer[13] = 0x00 #B                   
                    '''
                    my_buffer = [0x0] * 31
                    my_buffer[0] = 0xff
                    my_buffer[1] = 0x55
                    my_buffer[2] = 0x06
                    my_buffer[3] = 0x00
                    my_buffer[4] = 0x02
                    my_buffer[5] = 0x22
                    my_buffer[6] = 0x09
                    my_buffer[7] = 0x00
                    my_buffer[8] = 0x00
                    my_buffer[9] = 0x0a
                    '''
                    #out_report.set_raw_data(my_buffer)
                    #out_report.send()
                    out_report.send(my_buffer)
                    #device.close()                    
        except KeyboardInterrupt:
            print("\nBye")
            device.close
            sys.exit()

        """
0xff 0x55 0x6 0x0 0x2 0x22 0x9 0x0 0x0 0xa
        if msvcrt.kbhit():
            key = msvcrt.getch()
            key_number = ord(key)
            print(key_number)  # stroked key number
            if key_number == 113:
                break
            if key_number == 56:  # go ahead
                device.open()
                out_report = device.find_output_reports()[0]
                my_buffer = [0x0] * 31
                my_buffer[0] = 0x0
                my_buffer[1] = 0x02
                my_buffer[2] = 0x31
                my_buffer[3] = 0x0A
                out_report.set_raw_data(my_buffer)
                out_report.send(my_buffer)
                device.close()
            if key_number == 53:  # stop moving
                device.open()
                out_report = device.find_output_reports()[0]
                my_buffer = [0x0] * 31
                my_buffer[0] = 0x0
                my_buffer[1] = 0x02
                my_buffer[2] = 0x35
                my_buffer[3] = 0x0A
                out_report.set_raw_data(my_buffer)
                out_report.send(my_buffer)
                device.close()
            if key_number == 50:  # backwards
                device.open()
                out_report = device.find_output_reports()[0]
                my_buffer = [0x0] * 31
                my_buffer[0] = 0x0
                my_buffer[1] = 0x02
                my_buffer[2] = 0x32
                my_buffer[3] = 0x0A
                out_report.set_raw_data(my_buffer)
                out_report.send(my_buffer)
                device.close()
            if key_number == 52:  # turn left
                device.open()
                out_report = device.find_output_reports()[0]
                my_buffer = [0x0] * 31
                my_buffer[0] = 0x0
                my_buffer[1] = 0x02
                my_buffer[2] = 0x33
                my_buffer[3] = 0x0A
                out_report.set_raw_data(my_buffer)
                out_report.send(my_buffer)
                device.close()
            if key_number == 54:  # turn right
                device.open()
                out_report = device.find_output_reports()[0]
                my_buffer = [0x0] * 31
                my_buffer[0] = 0x0
                my_buffer[1] = 0x02
                my_buffer[2] = 0x34
                my_buffer[3] = 0x0A
                out_report.set_raw_data(my_buffer)
                out_report.send(my_buffer)
                device.close()
"""


# Motor Control Client in python

import matplotlib.pyplot as plt 
from statistics import mean 
def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = float(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()

from genref import genRef

import serial
ser = serial.Serial('/dev/tty.usbmodem101')
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('\n') # print a newline for formatting

    print('PICO MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: read current \td: add 1 \te: sum & difference \tf: set duty cycle \tg: set Kp and Ki \th: read Kp and Ki \tr: read mode \tq: quit') # '\t' is a tab
  
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate actionsource .venv/bin/activate

    # there is no switch() in python, using if elif instead
    if (selection == 'a'):
        amps_str = ser.read_until(b'\n')
        amps_float = float(amps_str)
        print('Current = ' + str(amps_float) + ' amps')
    elif (selection == 'd'):
        input_str = input('Enter an integer: ') 
        input_int = int(input_str)
        print('The number = ' + str(input_int)) 
        ser.write((str(input_int)+'\n').encode())
        n_str = ser.read_until(b'\n')
        n_int = int(n_str)
        print('Returned number = '+str(n_int)+'\n')
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    elif (selection == 'e'):
        input_str = input('Enter two integers separated by a space: ')
        input_list = list(map(int,input_str.split()))
        print('The numbers are ' + str(input_list[0]) + ' and ' + str(input_list[1]))
        ser.write((str(input_list[0])+' '+str(input_list[1])+'\n').encode())
        sum_str = ser.read_until(b'\n')
        sum_int = int(sum_str)
        print('Returned sum = ' + str(sum_int))
        diff_str = ser.read_until(b'\n')
        diff_int = int(diff_str)
        print('Returned difference = ' + str(diff_int))
    elif (selection == 'f'):
        input_str = input('Enter duty cycle in range -100.0 to 100.0: ')
        input_float = float(input_str)
        print('The duty cycle = ' + str(input_float))
        ser.write((str(input_float)+'\n').encode())
    elif (selection == 'g'):
        input_str = input('Enter Kp and Ki separated by a space: ')
        input_list = list(map(float,input_str.split()))
        print('Set Kp to ' + str(input_list[0]) + ' and Ki to ' + str(input_list[1]))
        ser.write((str(input_list[0])+' '+str(input_list[1])+'\n').encode())
    elif (selection == 'h'):
        output_str = ser.read_until(b'\n')
        output_list = list(map(float,output_str.split()))
        print('Kp = ' + str(output_list[0]) + ', Ki = ' + str(output_list[1]))
    elif (selection == 'p'):
        print('Set system to idle')
    elif (selection == 'r'):
        
        output_str = ser.read_until(b'\n')
        output_int = int(output_str)
        modes = ['IDLE', 'PWM', 'ITEST', 'HOLD', 'TRACK']
        print('Mode: ' + modes[output_int])
        break
    else:
        print('Invalid Selection ' + selection_endline)

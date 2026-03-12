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
    print('d: add 1 \t\te: sum & difference \ta: read current')
    print('f: set duty cycle \tg: set Kp_current and Ki_current \th: read Kp_current and Ki_current')
    print('k: ITEST \tt: read encoder counts \ty: read encoder angle \tz: zero encoder counts')
    print('i: set Kp_position, Ki_position, and Kd_position \t\tj: read Kp_position, Ki_position, and Kd_position')
    print('m: load step trajectory \tn: load cubic trajectory \to: track trajectory')
    print('r: read mode \t\tp: power off \tq: quit \tq: quit')
  
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # there is no switch() in python, using if elif instead
    if (selection == 'd'):
        # adds 1 to an integer
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        input_str = input('Enter an integer: ') 
        input_int = int(input_str)
        print('The number = ' + str(input_int)) 
        ser.write((str(input_int)+'\n').encode())
        n_str = ser.read_until(b'\n')
        n_int = int(n_str)
        print('Returned number = '+str(n_int)+'\n')
    elif (selection == 'e'):
        # adds and subtracts two integers
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
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
    elif (selection == 'a'):
        # reads current
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        amps_str = ser.read_until(b'\n')
        amps_float = float(amps_str)
        print('Current = ' + str(amps_float) + ' mA')
    elif (selection == 'f'):
        # sets duty cycle
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        input_str = input('Enter duty cycle in range -100.0 to 100.0: ')
        input_float = float(input_str)
        print('The duty cycle = ' + str(input_float))
        ser.write((str(input_float)+'\n').encode())
    elif (selection == 'g'):
        # sets Kp and Ki for current
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        input_str = input('Enter Kp and Ki separated by a space: ')
        input_list = list(map(float,input_str.split()))
        print('Set Kp to ' + str(input_list[0]) + ' and Ki to ' + str(input_list[1]))
        ser.write((str(input_list[0])+' '+str(input_list[1])+'\n').encode())
    elif (selection == 'h'):
        # reads out Kp and Ki for current
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        output_str = ser.read_until(b'\n')
        output_list = list(map(float,output_str.split()))
        print('Kp = ' + str(output_list[0]) + ', Ki = ' + str(output_list[1]))
    elif (selection == 'k'):
        # measures step response from -100mA to 100mA
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        read_plot_matrix()
    elif (selection == 't'):
        # reads encoder counts
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        counts_str = ser.read_until(b'\n')
        counts_int = int(counts_str)
        print('Encoder counts = ' + str(counts_int))
    elif (selection == 'y'):
        # reads encoder angle
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        angle_str = ser.read_until(b'\n')
        angle_float = float(angle_str)
        print('Encoder angle = ' + str(angle_float) + ' degrees')
    elif (selection == 'z'):
        # sets encoder counts to zero
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        print('Encoder counts set to zero')
    elif (selection == 'i'):
        # sets Kp, Ki, and Kd for position control
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        input_str = input('Enter Kp, Ki, and Kd separated by spaces: ')
        input_list = list(map(float,input_str.split()))
        print('Set Kp to ' + str(input_list[0]) + ', Ki to ' + str(input_list[1]) + ', and Kd to ' + str(input_list[2]))
        ser.write((str(input_list[0])+' '+str(input_list[1])+' '+str(input_list[2])+'\n').encode())
    elif (selection == 'j'):
        # reads out Kp, Ki, and Kd for position control
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        output_str = ser.read_until(b'\n')
        output_list = list(map(float,output_str.split()))
        print('Kp = ' + str(output_list[0]) + ', Ki = ' + str(output_list[1]) + ', Kd = ' + str(output_list[2]))
    elif (selection == 'm'):
        # loads step trajectory
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        trajectory = genRef('step')

        ser.write(f"{len(trajectory)}\n".encode())

        for position in trajectory:
            ser.write(f"{position}\n".encode())
    elif (selection == 'n'):
        # loads step trajectory
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        trajectory = genRef('cubic')

        ser.write(f"{len(trajectory)}\n".encode())
        
        for position in trajectory:
            ser.write(f"{position}\n".encode())
    elif (selection == 'o'):
        # tracks trajectory
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        read_plot_matrix()
    elif (selection == 'r'):
        # reads out system status
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        output_str = ser.read_until(b'\n')
        output_int = int(output_str)
        modes = ['IDLE', 'PWM', 'ITEST', 'HOLD', 'TRACK']
        print('Mode: ' + modes[output_int])
    elif (selection == 'p'):
        # powers off the system
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        print('Set system to idle')
    elif (selection == 'q'):
        # quits system
        # send the command to the PIC32
        ser.write(selection_endline.encode()) # .encode() turns the string into a char array
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    else:
        # invalid input
        print('Invalid Selection ' + selection)
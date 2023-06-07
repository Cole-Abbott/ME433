# get a line of raw bitmap and plot the components
import serial
from scipy.signal import butter,filtfilt
import numpy as np

ser = serial.Serial('/dev/tty.usbmodem101',230400) # the name of your Pico port
print('Opening port: ')
print(ser.name)

ser.write(b'hi\r\n') # send a newline to request data
data_read = ser.read_until(b'\n',50) # read the echo

sampnum = 0
index = 0
raw = []
reds = []
greens = []
blues = []
bright = []

# moving average filter
def MAF(data, n):
    filteredData = []
    n = int (n/2)
    for i in range(n,len(data)-n): 
        # take the mean of the window
        filteredData.append(np.mean(data[i-n:i+n]))
    return filteredData

def derivative(data):
    deriv = []
    for i in range(2,len(data)-2):
        deriv.append(data[i+2] - data[i-2])
    return deriv

def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='high', analog=False)
    return b, a

def butter_highpass_filter(data, cutoff, fs, order=5):
    b, a = butter_highpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    return y


# Pico sends back index and raw pixel value
while sampnum < 60: # width of bitmap
    data_read = ser.read_until(b'\n',50) # read until newline
    data_text = str(data_read,'utf-8') # convert bytes to string
    data = list(map(int,data_text.split())) # convert string to values

    if(len(data)==2):
        index = data[0]
        raw.append(data[1])
        reds.append(((data[1]>>5)&0x3F)/0x3F*100) # red value is middle 6 bits
        greens.append((data[1]&0x1F)/0x1F*100) # green value is rightmost 5 bits
        blues.append(((data[1]>>11)&0x1F)/0x1F*100) # blue vale is leftmost 5 bits
        bright.append((data[1]&0x1F)+((data[1]>>5)&0x3F)+((data[1]>>11)&0x1F)) # sum of colors
        sampnum = sampnum + 1

# print the raw color as a 16bit binary to double check bitshifting
#for i in range(len(reds)):
#    print(f"{raw[i]:#018b}")

# lowpass filter the data
reds = MAF(reds,8)
greens = MAF(greens,8)
blues = MAF(blues,8)
bright = MAF(bright,8)


# high pass filter the data
reds = butter_highpass_filter(reds, 1.5, 52, 10)
greens = butter_highpass_filter(greens, 1.5, 52, 10)
blues = butter_highpass_filter(blues, 1.5, 52, 10)
bright = butter_highpass_filter(bright, 1.5, 52, 10)
#



# plot the colors 
import matplotlib.pyplot as plt 
x = range(len(reds)) # time array

plt.plot(x,reds,'r*-',x,greens,'g*-',x,blues,'b*-', x,bright,'k*-')
plt.ylabel('color')
plt.xlabel('position')
plt.show()

# be sure to close the port
ser.close()
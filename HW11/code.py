from ulab import numpy as np # to get access to ulab numpy functions
#import matplotlib.pyplot as plt # to plot the sin waves
#import numpy as np # to get access to numpy functions

#declare 3 arrays of sin waves with different frequencies
# 1.0 Hz
t1 = np.arange(0, 2.56, 0.01)

y1 = np.sin(2*np.pi*t1)

# 5.0 Hz
t2 = np.arange(0, 2.56, 0.01)
y2 = np.sin(10*np.pi*t2)

# 30.0 Hz
t3 = np.arange(0, 2.56, 0.01)
y3 = np.sin(60*np.pi*t3)



#combine the 3 sin waves into 1 array
y = y1 + y2 + y3
t = t1

#take fft of the 3 sin waves
n = len(y) # length of the signal

Fs = n/t[-1] # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector 
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y) # fft computing and normalization

#print out the fft values
for t, y in zip(frq, abs(Y)):
    print(t, y)

#Y = Y[range(int(n/2))]

#fig, (ax1, ax2) = plt.subplots(2, 1)
#ax1.plot(t,y,'b')
#ax1.set_xlabel('Time')
#ax1.set_ylabel('Amplitude')
#ax2.loglog(frq,abs(Y),'b') # plotting the fft
#ax2.set_xlabel('Freq (Hz)')
#ax2.set_ylabel('|Y(freq)|')
#plt.show()


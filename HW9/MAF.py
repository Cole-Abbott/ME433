import csv
import numpy as np
import matplotlib.pyplot as plt


t = [] # column 0
data = [] # column 1
dataSet = 'sigA.csv'
def main():
    with open(dataSet) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data.append(float(row[1])) # second column



    #filter data with moving average filter
    x = 15
    mafData = MAF(data, x)

    mafY, mafFrq = fft(t, mafData) # the data to make the fft from
    Y, frq = fft(t, data) # the data to make the fft from

 
    #plot the data and fft
    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,data,'black')
    ax1.plot(t,mafData,'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f'{dataSet}, x={x}')
    ax2.loglog(frq,abs(Y),'black') # plotting the unfiltered fft
    ax2.loglog(mafFrq,abs(mafY),'r') # plotting the filtered fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()




def fft(t, data):
    y = data # the data to make the fft from
    n = len(y) # length of the signal
    Fs = n/t[-1] # sample rate
    Ts = 1.0/Fs; # sampling interval
    ts = np.arange(0,t[-1],Ts) # time vector 
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]
    return Y, frq

# moving average filter
def MAF(data, n):
    filteredData = []
    for i in range(len(data)):
        if i < n: # if the index is less than the window size, skip
            filteredData.append(data[i])
            continue
        else: # take the mean of the window
            filteredData.append(np.mean(data[i-n:i]))
    return filteredData


if __name__ == "__main__":
    main()
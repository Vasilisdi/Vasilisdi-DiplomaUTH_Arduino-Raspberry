import numpy as np
import matplotlib.pyplot as plt
import math
import scipy.signal as signal

def calculate_amplitude(col_1, col_2, col_3):
    return math.sqrt(col_1**2 + col_2**2 + col_3**2)

def compute_fft(data, Fs, L):
    fft_result = np.fft.fft(data,L)
    P2 = np.abs(fft_result[1:]/L)
    P1 = P2[:L//2]
    P1[1:-1] = 2*P1[1:-1]
    return P1, Fs/L*np.arange(1, L//2+1)

def compute_envelope(data):
    analytic_signal = signal.hilbert(data)
    envelope = np.abs(analytic_signal)
    return envelope

def plot_fft(P1, f, title, ylabel):
    plt.figure()
    plt.plot(f, P1, linewidth=3)
    plt.title(title)
    plt.xlabel("f (Hz)")
    plt.ylabel(ylabel)
    plt.show()


def unbalancing(frequency,rpm,spectrum,threshold): 
    #index = f.index(rpm)  to be used in main.py
    index = np.where((frequency > 0.999 * rpm) & (frequency < 1.001 * rpm))
    #print(index)
    frq = frequency[index]
    amplitude = spectrum[index]

    if amplitude > threshold:
        status = 'Shaft needs to be balanced'
    else:
        status = 'Balancing is ok'
    
    return f"RPM: {frq}, Amplitude: {amplitude}, Status: {status}"

import numpy as np
import matplotlib.pyplot as plt
import math
import scipy.signal as signal

def calculate_magnitude(col_1, col_2, col_3):
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

def plot_td(historicalData, t, title, ylabel):
    plt.figure()
    plt.plot(t, historicalData , linewidth=3)
    plt.title(title)
    plt.xlabel("Time [s]")
    plt.ylabel(ylabel)
    plt.show()


def plot_fft_and_td(f, P1, time, data, title_fft, ylabel_fft, ylabel_td):
    fig, axs = plt.subplots(2, 1, figsize=(10, 6))

    # FFT
    axs[0].plot(f, P1, linewidth=2)
    axs[0].axhline(y=1, color='r', linestyle='--', linewidth=1)
    axs[0].set_title(title_fft)
    axs[0].set_xlabel("Frequency [Hz]")
    axs[0].set_ylabel(ylabel_fft)
    axs[0].set_xlim(0, 300)  # Restrict x-axis to 0–300 Hz
    #axs[0].set_ylim(0, 0.1)  # Restrict x-axis to 0–300 Hz
    axs[0].grid(True)

    # Time-domain
    axs[1].plot(time, data, linewidth=2)
    axs[1].set_title("Time-domain Signal")
    axs[1].set_xlabel("Time [s]")
    axs[1].set_ylabel(ylabel_td)
    axs[1].grid(True)

    plt.tight_layout()
    plt.show()


def unbalancing(frequency, rpm, spectrum, threshold): 
    index = np.where((frequency > 0.999 * rpm) & (frequency < 1.001 * rpm))
    frq = frequency[index]
    amplitude = spectrum[index]

    if amplitude.max() > threshold:  # Use max amplitude
        status = 'Shaft needs to be balanced'
    else:
        status = 'Balancing is ok'
    
    return f"RPM: {frq}, Amplitude: {amplitude}, Status: {status}"


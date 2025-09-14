import pandas as pd
from sourceCode.data_processing import compute_fft, plot_fft_and_td
import matplotlib.pyplot as plt

csv_file = r"testing\raw_test_data\outer_race_waveform\O800.csv"

# Load the dataset
df = pd.read_csv(csv_file)

# Extract time and amplitude
time = df['time_sec'].values
signal = df['amplitude'].values

# Sampling frequency (calculated from time step)
dt = time[1] - time[0]
Fs = 51200
L = len(signal)


ylabels = ["|P1(f)| - Magnitude [g]", "|P1X(f)|", "|P1Y(f)|", "|P1Z(f)|"]
ylabelst = ["Amplitude [g]" , "X[t]" , "Y[t]" , "Z[t]"]

for i, data in enumerate([signal]):
    P1, f = compute_fft(data, Fs, L)
    titles = [
        "Single-Sided Magnitude Spectrum w/ rotational speed 24,37[Hz] (Inner race fault X3,57)",
    ]

    print(len(P1))

    print(f"Sampling rate: {Fs} Hz")
    print(f"Frequency resolution: {Fs/L} Hz per bin")

    plot_fft_and_td(f, P1, time, data, titles[i], ylabels[i], ylabelst[i])
import pandas as pd
import numpy as np
from sourceCode.data_processing import compute_fft, plot_fft_and_td, calculate_magnitude
import matplotlib.pyplot as plt

csv_file = r"testing\raw_test_data\ball_fault_waveform\bearing_000_Ch08_100g_PE_Acceleration.csv"

# Load the dataset

# Load dataset (no headers in your file, so add them manually)
df = pd.read_csv(csv_file, header=None, names=["time", "x_g", "y_g", "z_g"])

# Extract signals
time = df["time"].values
x = df["x_g"].values
y = df["y_g"].values
z = df["z_g"].values
m = []
for i in range(len(x)):
    m.append(calculate_magnitude(x[i], y[i], z[i]))
m = np.array(m) 

# Sampling frequency (calculated from time step)
dt = time[1] - time[0]
Fs = 20000
L = len(x)


ylabels = ["|P1(f)| - Magnitude [g]", "|P1X(f)| - Magnitude [g]", "|P1Y(f)| - Magnitude [g]", "|P1Z(f)| - Magnitude [g]"]
ylabelst = ["Amplitude [g]" , "X[t]" , "Y[t]" , "Z[t]"]

for i, data in enumerate([m, x, y, z]):
    P1, f = compute_fft(data, Fs, L)
    titles = [
        "Single-Sided Magnitude Spectrum w/ rotational speed 50[Hz] (missalignment)"
    ]

    print(len(P1))

    print(f"Sampling rate: {Fs} Hz")
    print(f"Frequency resolution: {Fs/L} Hz per bin")

    plot_fft_and_td(f, P1, time, data, titles, ylabels[i], ylabelst[i])
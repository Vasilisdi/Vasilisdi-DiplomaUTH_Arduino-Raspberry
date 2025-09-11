import time
import numpy as np
from sourceCode.utils import load_config
from sourceCode.data_processing import compute_envelope, compute_fft, calculate_magnitude, plot_fft, plot_td, unbalancing
from sourceCode.serial_communication import setup_serial_connection, read_serial_data


# Load configuration
config = load_config()

baud_rate = config['serial']['baud_rate']
#port = config['serial']['port_linux']   #When working on Rabperry Pi or Fedora, there should be a shift
port = config['serial']['port_windows']
rpm = config['rpm']
threshold = config['threshold']
time_window = config['time_window']
ser = setup_serial_connection(port, baud_rate)

time.sleep(1)
ser.reset_input_buffer

history = []
history_X = []
history_Y = []
history_Z = []
time_window = config['time_window']
timestamps = []
start_time = time.time()


try:
    while time.time() - start_time < time_window:
        timestamp = time.time()
        timestamps.append(timestamp)
        col_1, col_2, col_3 = read_serial_data(ser)
        amplitude = calculate_magnitude(col_1, col_2, col_3)
        history.append(((amplitude*(3.3 / 1023))-1.65)/0.33)
        history_X.append(((col_1*(3.3 / 1023))-1.65)/0.33)
        history_Y.append(((col_2*(3.3 / 1023))-1.65)/0.33)
        history_Z.append(((col_3*(3.3 / 1023))-1.65)/0.33)

    # Calculate the actual sampling rate
    # Calculate time differences between successive data points
    time_diffs = np.diff(timestamps)
    Fs = 1 / np.mean(time_diffs)

    L = config['Len']   #number of FFT points - output vector length
    ylabels = ["|P1(f)|", "|P1X(f)|", "|P1Y(f)|", "|P1Z(f)|"]
    ylabelst = ["Magnitude[t]" , "X[t]" , "Y[t]" , "Z[t]"]

    for i, data in enumerate([history, history_X, history_Y, history_Z]):
        P1, f = compute_fft(data, Fs, L)
        titles = [
            "Single-Sided Amplitude Spectrum of sqrt(X(t)^2 + Y(t)^2 + Z(t)^2)",
            "Single-Sided Amplitude Spectrum of X(t)",
            "Single-Sided Amplitude Spectrum of Y(t)",
            "Single-Sided Amplitude Spectrum of Z(t)"
        ]

        #print(f)

        print(f"Sampling rate: {Fs} Hz")
        print(f"Frequency resolution: {Fs/L} Hz per bin")


        print(unbalancing(f,rpm,P1,threshold))
        print(f"Frequency domain upper value: {f[-1]} Hz")

        plot_fft(P1, f, titles[i], ylabels[i])
        plot_td(data, np.linspace(0, time_window, len(data)) , "time [s]", ylabelst[i])

    for i,data in enumerate([history, history_X, history_Y, history_Z]):
        Envelope = compute_envelope(data)
        #print(Envelope)

    

except KeyboardInterrupt:
    ser.close()
    

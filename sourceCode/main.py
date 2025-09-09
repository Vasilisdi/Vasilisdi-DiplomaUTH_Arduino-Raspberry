from datetime import datetime, timedelta, timezone
from api import VibrationMonitoringAPI
from data_processing import calculate_magnitude, compute_fft
from serial_communication import setup_serial_connection, read_serial_data
from utils import get_api_key, load_config
import numpy as np

# Load configuration
config = load_config()

# Load API key from environment variable
api_key = get_api_key()

if not api_key:
    raise ValueError("API key not found. Please set the API_KEY environment variable.")

vibration_api = VibrationMonitoringAPI()

baud_rate = config['serial']['baud_rate']
port = config['serial']['port_linux']
#port = config['serial']['port_windows']

ser = setup_serial_connection(port, baud_rate)

history = []
history_X = []
history_Y = []
history_Z = []
time_window = config['time_window']
timestamps = []
start_time = datetime.now(timezone.utc)
time_delta = timedelta(seconds=time_window)

try:
    while datetime.now(timezone.utc) - start_time < time_delta:
        timestamp = datetime.now(timezone.utc)
        timestamps.append(timestamp.timestamp())
        col_1, col_2, col_3 = read_serial_data(ser)
        amplitude = calculate_magnitude(col_1, col_2, col_3)
        history.append(((amplitude*(5 / 1023))-2.5)/0.3)
        history_X.append(((col_1*(5 / 1023))-2.5)/0.3)
        history_Y.append(((col_2*(5 / 1023))-2.5)/0.3)
        history_Z.append(((col_3*(5 / 1023))-2.5)/0.3)



    # Calculate the actual sampling rate
    # Calculate time differences between successive data points
    time_diffs = np.diff(timestamps)
    Fs = 1 / np.mean(time_diffs)
    #L = len(history)
    L = config['Len']   #number of FFT points - output vector length

    print(f" raw data : len={len(history_X)}")

    for i, data in enumerate([history, history_X, history_Y, history_Z]):
        values, f = compute_fft(data, Fs, L)
        titles = [
            "Single-Sided Amplitude Spectrum of sqrt(X(t)^2 + Y(t)^2 + Z(t)^2)",
            "Single-Sided Amplitude Spectrum of X(t)",
            "Single-Sided Amplitude Spectrum of Y(t)",
            "Single-Sided Amplitude Spectrum of Z(t)"
        ]
        ylabels = ["|P1(f)|", "|P1X(f)|", "|P1Y(f)|", "|P1Z(f)|"]
        #plot_fft(values, f, titles[i], ylabels[i])

        coordinate = ["Magnitude", "X", "Y", "Z"]

        print(f"{coordinate[i]}: len={len(values)}")

        api = VibrationMonitoringAPI()

        status_code = api.send_measurement('Test', coordinate[i], values.tolist(), f.tolist() , start_time , timestamp)



except KeyboardInterrupt:
    ser.close()

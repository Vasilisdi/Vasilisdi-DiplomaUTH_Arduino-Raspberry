import serial
import time

def setup_serial_connection(port, baud_rate):
    ser = serial.Serial(port, baud_rate, timeout=1.0)
    time.sleep(1)
    ser.reset_input_buffer()
    return ser

def read_serial_data(ser):
    ser.write('rasp \n'.encode())
    while ser.in_waiting <= 0:
        time.sleep(0.001)
    response = ser.readline().decode('utf-8').rstrip()
    col_1, col_2, col_3 = map(float, response.split(','))
    return col_1, col_2, col_3
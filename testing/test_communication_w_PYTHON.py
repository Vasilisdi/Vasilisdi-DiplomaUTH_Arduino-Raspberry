import serial
import struct
import time

ser = serial.Serial('COM5', 115200, timeout=0.1)
time.sleep(2) 

xs = []
ys = []
zs = []

buffer = bytearray()
start = time.time()
while time.time() - start < 60:
    data = ser.read(60)
    buffer.extend(data)
    if len(buffer) == 60:
        while len(buffer) >= 6:
            frame = buffer[:6]
            buffer = buffer[6:]
            x , y , z = struct.unpack('>HHH', frame)
            xs.append(x)
            ys.append(y)
            zs.append(z)
    elif len(data) !=60:
        print("Incomplete packet")

ser.close()

for i in range(len(xs)):
    print(xs[i], ys[i], zs[i])
print("The length of the vector is: ", len(xs))
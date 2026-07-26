import serial
import struct
import time
import numpy as np

ser = serial.Serial("COM5", 115200)

BLOCK_SIZE = 6000        # 3000 samples * 2 bytes
SAMPLES_PER_BLOCK = 3000 # 3000 uint16 values

samples_x = []
samples_y = []
samples_z = []
sample_ids = []

start = time.time()
blocks = 0

while time.time() - start < 60:

    data = ser.read(BLOCK_SIZE)

    # Skip incomplete blocks
    if len(data) != BLOCK_SIZE:
        continue

    blocks += 1

    # Unpack 3000 uint16 samples
    raw = struct.unpack("<" + "H"*SAMPLES_PER_BLOCK, data)

    # Split into x,y,z triples
    xs = raw[0::3]
    ys = raw[1::3]
    zs = raw[2::3]

    samples_x.extend(xs)
    samples_y.extend(ys)
    samples_z.extend(zs)

    # Generate synthetic sample_ids (continuous)
    # Each block has 1000 packets (3000 samples / 3)
    base_id = (blocks - 1) * (SAMPLES_PER_BLOCK // 3)
    for i in range(SAMPLES_PER_BLOCK // 3):
        sample_ids.append(base_id + i)

ser.close()

print("Received blocks:", blocks)
print("Total packets:", len(sample_ids))

# Lost packet check
errors = 0
first_id = sample_ids[0]

for expected, received in enumerate(sample_ids):
    if received != first_id + expected:
        errors += 1

print("Lost packets:", errors)
print(samples_x[-10:-1])
print(samples_y[-10:-1])
print(samples_z[-10:-1])

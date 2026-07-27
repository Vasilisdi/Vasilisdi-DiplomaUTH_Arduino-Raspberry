# Vasilisdi-DiplomaUTH

## About This Project

This repository contains the full software implementation of my UTH MSc diploma thesis on **continuous machine vibration monitoring** and **FFT‑based fault detection**.

It includes two complete architectures:

### 1. Python + Arduino/Raspberry Pipeline  
A full end‑to‑end vibration analysis workflow using Python, MQTT, Telegraf, and InfluxDB.  
It supports:
- High‑frequency vibration acquisition  
- Real‑time FFT feature extraction  
- Dataset validation and testing scripts  
- Supabase integration for remote data posting  

This implementation is used for concept validation, dataset testing, and periodic monitoring.

### 2. STM32 + C++ + FreeRTOS + DMA Pipeline (version2 branch)  
A high‑speed industrial architecture designed for **continuous waveform streaming** and **dense spectral analysis**.  
It features:
- Custom ADC + DMA double‑buffer acquisition  
- FreeRTOS synchronization  
- USB CDC high‑throughput transmission  
- C++ FFT processing  
- MQTT → Telegraf → InfluxDB → Grafana telemetry chain  

This architecture achieves significantly higher sampling rates and stable real‑time acquisition suitable for rotating machinery diagnostics.

---


#  Project Introduction

This repository contains the software implementation of my diploma thesis on **continuous machine vibration monitoring** and **FFT‑based fault detection**.  
The system collects high‑frequency vibration data from an STM32 microcontroller, processes it in real time, and streams both waveform and frequency‑domain features through **MQTT → Telegraf → InfluxDB**, enabling live dashboards and predictive maintenance analytics.

The full theoretical background, methodology, and detailed explanation of the vibration analysis pipeline are available in my technical report:

👉 **Full Theory & Documentation:**  
https://github.com/Vasilisdi/Technical-Report

###  Thesis Background (Summary)

This thesis investigates the design and implementation of a year‑round vibration monitoring system capable of detecting mechanical faults in rotating machinery.  
Using **Fast Fourier Transform (FFT)** techniques, the system identifies characteristic fault signatures and supports **predictive maintenance** strategies with low‑cost hardware such as the ADXL335 accelerometer.

### 🎯 Key Outcomes

- ✅ Real‑time fault detection in rotating machinery  
- ✅ Data‑driven preventive maintenance strategies  
- ✅ Cost‑effective FFT‑based analysis pipeline  
- ✅ End‑to‑end IIoT architecture using STM32, MQTT, Telegraf, and InfluxDB  



# About Repositories
In the repository, there is two different approaches for the same problem, separated in two different branches - main and version2. In the main branch there is the "results" folder containing the results of the testing phase and the deployment phase on a rotating fan. While the "dataset" folder is used for validation and visual checks.

This repository contains 2 different running sections. The first section is this of testing. There is the files test_data_from_datasets1.py and test_data_from_datasets2.py.  The frequenct spectra of the defective machine waveforms are to be ploted on some figure for visual analysis and illustration. (following detailed instructions on this).

In addition to this, there is the sourceCode folder, containing the main.py file and some other auxiliary files. This file is designed for the case of the continuous monitoring case, using cronjob set at collecting data four times a day. (further instructions bellow)

- test_data_from_datasets1.py for test datasets model validation.
- test_data_from_datasets2.py for test datasets model validation.
- main.py for real-life machine applications.

While in the "version2" branch encompasses a more industial-based appoach that will be elaborated in a following section.


# Testing Phase Running


## Prerequisites

Ensure you have [Poetry](https://python-poetry.org/) installed on your system. If Poetry is not installed, you can install it using the following command:

```sh
curl -sSL https://install.python-poetry.org | python3 -
```

Additionally, ensure that Poetry’s binary path is available in your system’s `PATH` variable:

```sh
export PATH="$HOME/.local/bin:$PATH"
```

To make this change permanent (not mandatory), add the above line to your `~/.bashrc` or `~/.zshrc` file:

```sh
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc  # or source ~/.zshrc if using Zsh
```

Also ensure the an .env file is created following the .env.example file as a template. This is going to be filled with sensitive credentials of your supabase project.


## Installation

Install `poetry`. Install the dependencies using `poetry`:

```sh
poetry install
```

## Environment Variables and Configuration

Create a `.env` similar to `.env.sample` and set the values in the file. Modify the `config/config.yaml` file if needed to set the required values.

## Activating the virtual environment

```sh
poetry shell
```

An alternative to activating the virtual environment created by Poetry is running the following command (applicable in newer versions of poetry):

```sh
source $(poetry env info --path)/bin/activate
```

This ensures that your Python environment is properly configured for the project.


### Troubleshooting

- If `poetry` is not recognized, restart your terminal or re-run the `export PATH` command.
- If the virtual environment is not found, recreate it using:

### Reinstall all dependencies

Remove the existing Poetry environment
```sh
poetry env remove python
```
And reinstall
```sh
poetry install  # Installs all necessary libraries
```

### Test data posting to Supabase
```sh
poetry run python -c "from sourceCode.api import VibrationMonitoringAPI; api = VibrationMonitoringAPI(); api.send_measurement(sensor_name='Test Sensor', coordinate='TEST', values=[1, 2, 3, 4, 5])" # Verifies the ability to post data to Supabase
```


### Locate the project folder in Raspberry Pi and run a command that resembles the following (via SSH):

```sh
cd repos/Vibration-Analysis-Project/raspberry
PYTHONPATH=/home/VASILIS/repos/Vibration-Analysis-Project/raspberry poetry run python3.12 tests/test_methods.py

or

cd repos/Diploma\ Thesis/raspberry
PYTHONPATH=/home/VASILIS/repos/Diploma\ Thesis/raspberry poetry run python3.12 testing/test_methods.py

and

in the case of probing on a PC use relative imports and ensure everything is set up as part of a package and Python locates the folder and the file.
poetry run python -m testing.test_methods
```

## Debugging Poetry Import Issues

If you encounter issues with module imports, run the following command to debug:

```sh
poetry run python -c "from sourceCode.utils import load_config; print(load_config())"
```

This command directly imports and prints the result of `load_config()` from `sourceCode.utils`. If an error occurs:

- Ensure `sourceCode` exists and contains an `__init__.py` file.
- Check that `sourceCode.utils` is accessible within the Poetry environment.
- Verify `PYTHONPATH` includes the correct project directory.
- Run `poetry shell` before executing Python commands to ensure the correct environment is active.

### Debugging API Configuration

To verify that the API key and Supabase URL are correctly loaded, run:

```sh
poetry run python -c "from sourceCode.api import VibrationMonitoringAPI; api = VibrationMonitoringAPI(); print(api.purl, api.api_key)"
```

This command initializes the `VibrationMonitoringAPI` class and prints the loaded API key and Supabase URL.

As articulated above, there is also the option of checking the data posting possiblility:
```sh
poetry run python -c "from sourceCode.api import VibrationMonitoringAPI; api = VibrationMonitoringAPI(); api.send_measurement(sensor_name='Test Sensor', coordinate='TEST', values=[1, 2, 3, 4, 5])" # Verifies the ability to post data to Supabase
```


## Deactivating the virtual environment

```sh
exit
```



# Testing and Validation
## Running Tests files

After setting up the environment, you can execute the test suite by running:

```sh
poetry run python -m testing.test_methods
```

This command ensures that Python runs the `test_methods.py` module inside the `testing` package within the Poetry environment.



## Concept validation

To run tests for processing vibration datasets:

```sh
poetry run python -m testing.test_data_from_datasets1
poetry run python -m testing.test_data_from_datasets2
```

These commands are used for the faulty machinery datasets included in the folders:

-Test processing for the VBL-VA001 dataset "poetry run python -m testing.test_data_from_datasets1"
-Test processing for the HUST Bearing dataset "poetry run python -m testing.test_data_from_datasets2"

Data files are mixed but they can be easily identified since HUST Bearing dataset uses the following format "I802" while the VBL-VA001 dataset is formated like so: "ub_06_000-3_Ch08_100g_PE_Acceleration".

This executable file have as an input the time-domain waveforms and convert data into the frequency domain.







# Actual - Raspberry Connected to Arduino Phase

In this case, the main script is to be executed so as to post real data on the supabase:

```sh
poetry run python sourceCode/main.py
```



# Version 2 — STM32 + C++ + MQTT + InfluxDB + Grafana (Real-Time Streaming) - Amendment of the original project

The repository includes a second, fully independent architecture located in the `version2` branch as well.

This implementation was designed to overcome the limitations of the original
Arduino + Raspberry Pi pipeline and enable significantly higher acquisition rates,
continuous waveform streaming, and industrial-grade telemetry visualization.

The `version2` branch introduces a complete real-time vibration monitoring pipeline
using:

- STM32 high-speed acquisition
- C++ signal processing
- MQTT telemetry streaming
- InfluxDB time-series storage
- Grafana dashboards

This architecture is intended for high-frequency rotating machinery monitoring,
FFT spectral analysis, and predictive maintenance experimentation.

---

# Overview of the version2 architecture

Accelerometer  
      ↓  
STM32 microcontroller (high-speed ADC sampling)  
      ↓ Serial communication (USB)  
PC Application (C++ waveform collector and FFT processor)  
      ↓  
MQTT Broker (telemetry streaming)  
      ↓  
Telegraf (MQTT → InfluxDB ingestion)  
      ↓  
InfluxDB (time-series storage)  
      ↓  
Grafana (real-time dashboards)

---

# Why STM32 [4,1kHz] instead of Arduino [300Hz]

The original Arduino-based implementation was suitable for:
- proof-of-concept testing
- low-frequency monitoring
- periodic acquisition
- lightweight deployments

However, it became insufficient for:
- high sampling frequencies
- dense waveform acquisition
- large FFT windows
- continuous real-time streaming

The STM32 architecture significantly improves:

- ADC acquisition speed
- serial throughput
- sampling stability
- waveform density
- FFT resolution
- real-time responsiveness

The system now reaches approximately a range of 4100 Hz of sampling frequency [0-4100 Hz] and this is firmly related to the capacity of stm32, which produces extremely dense vibration waveforms and significantly more accurate frequency-domain analysis. While Arduino may only reach frequency spectra up to 300 Hz. 

However, it became insufficient for:
- improved bearing fault visibility
- clearer harmonic analysis
- higher FFT frequency resolution
- better transient detection


# High-Frequency Acquisition Performance (version2 branch)

## Overview

This project implements a high-speed analog data acquisition system based on an **STM32F407 microcontroller** using:

- Custom ADC driver
- Custom DMA driver configuration
- DMA double-buffering
- FreeRTOS task synchronization
- Binary semaphores
- USB CDC/UART data transmission

The purpose of the system is to continuously acquire analog sensor data, transfer the samples efficiently using DMA without blocking the CPU, temporarily store the acquired data in memory buffers, and transmit the collected data to a host computer.

The complete acquisition chain is:

```text
Analog Sensor
      |
      v
ADC Peripheral
      |
      v
DMA Controller
      |
      v
Double Buffer Memory System
      |
      v
DMA Interrupt (ISR)
      |
      v
Binary Semaphore Notification
      |
      v
FreeRTOS Task Wake-Up
      |
      v
CPU Processes Completed Buffer
      |
      v
USB CDC Transmission
      |
      v
PC Application
```


The system contains a dedicated acquisition/transmission task.

The task remains blocked using `xSemaphoreTake()` until DMA completes a buffer transfer.

After receiving the semaphore notification, the task:

1. Retrieves the completed buffer.
2. Packages the ADC samples.
3. Transmits the data through USB CDC/UART.
4. Returns to the blocked state waiting for the next DMA event.

---

# Hardware

## Microcontroller

- STM32F407VG
- ARM Cortex-M4 CPU
- 12-bit ADC
- DMA controller
- USB OTG FS peripheral

## ADXL335 Accelerometer

The system uses the Adafruit ADXL335 analog accelerometer breakout.

The sensor provides three analog outputs:

- X-axis
- Y-axis
- Z-axis

The breakout is powered from the STM32 3.3V supply.

The analog outputs are directly connected to the STM32 ADC inputs.

The ADXL335 outputs are centered around half of the supply voltage:

```text
0g output:

X ≈ 1.65V
Y ≈ 1.65V
Z ≈ 1.65V
```
---

# Software Architecture

The software is written using:

- Bare-metal peripheral configuration
- Custom embedded drivers
- FreeRTOS real-time operating system

The system does not rely on STM32 HAL ADC/DMA drivers. The ADC and DMA peripherals are configured manually through register-level programming.

This provides:

- Better understanding of peripheral operation
- Lower software overhead
- Precise control over timing
- Deterministic real-time behavior

```md
STM32/
│
├── Core/
│   ├── Inc/
│   │   ├── adc.h
│   │   ├── exti.h
│   │   ├── uart.h
│   │   └── main.h
│   │
│   └── Src/
│       ├── main.c             # Application entry point
│       ├── adc.c              # ADC + DMA driver
│       ├── exti.c             # External interrupt driver
│       ├── uart.c             # UART communication
│       └── freertos.c         # RTOS tasks
│
├── Drivers/                   # Vendor libraries
├── Middlewares/               # FreeRTOS & USB
├── USB_Device/                # USB CDC stack
└── STM32F407.ioc
```

---

# Custom ADC Driver

## adc_init()

The function `adc_init()` is responsible for configuring the ADC peripheral.

The ADC initialization performs:

- Enabling the ADC peripheral clock
- Configuring the GPIO pin as analog input
- Selecting the ADC channel
- Configuring ADC resolution
- Enabling continuous conversion mode
- Configuring ADC sampling parameters
- Starting ADC operation

The ADC converts the analog input signal into digital values.

The STM32F407 ADC is a 12-bit converter.

The ADC conversion range is determined by the ADC reference voltage (VDDA/VREF+).

For the standard STM32 3.3V supply configuration:

```text
0V       -> ADC value 0
3.3V     -> ADC value 4095
```

Each ADC conversion produces a 16-bit memory value containing the 12-bit ADC result.

---

# DMA Configuration

## Purpose of DMA

The DMA controller allows ADC data transfers without requiring continuous CPU intervention.

Instead of:

CPU
|
v
Start ADC
|
v
Wait conversion
|
v
Read ADC register
|
v
Store data
|
v
Repeat



the system operates as:
ADC:
Convert sample

DMA:
Automatically move ADC result to RAM

CPU:
Remains free



This reduces CPU load and guarantees stable acquisition timing.


## DMA Double Buffer System

The system uses DMA double buffering.

Two memory areas are allocated:

Buffer 1:

```c
adc_buffer[]
```
Buffer 2:

```c
adc_buffer_spare[]
```

The DMA controller continuously alternates between these two memory regions.

Example operation:
Time 0:

DMA writes:

Buffer 1 <---------------- ADC

CPU:

Idle
Waiting



After Buffer 1 is full:
DMA switches:

Buffer 2 <---------------- ADC

CPU:

Processes Buffer 1


The process continues, DMA fills one buffer, while the CPU processes the other buffer.



This parallel operation allows continuous ADC acquisition while the CPU processes and transmits previously completed buffers. Data loss is avoided as long as the CPU processing and transmission time remains shorter than the DMA buffer refill period.


# DMA Interrupt Service Routine (ISR)

## DMA2_Stream0_IRQHandler()

The DMA interrupt is triggered when a buffer transfer is completed.

The ISR performs the following operations:

1. Checks if the DMA transfer complete flag is set.

2. Clears the DMA interrupt flag.

3. Updates the DMA transfer counter.

4. Determines which memory buffer has completed.

5. Stores the completed buffer address.

6. The completed DMA buffer is identified using the DMA current target (CT) bit. The ISR stores the address of the completed memory region in readyBuffer.

7. Releases a FreeRTOS binary semaphore, allowing the processing task to access readyBuffer while DMA continues writing into the other memory region.

The ISR does not process the data.

Its only responsibility is:


DMA completed transfer
|
v
Notify waiting task



This keeps interrupt execution time minimal.

---

# Buffer Synchronization

The completed buffer is passed to the processing task through a shared pointer: readyBuffer


The CPU always works on the buffer that DMA has already finished writing.

The active DMA buffer is never modified by the CPU.

This creates a safe producer-consumer architecture:

Producer:

ADC + DMA
    |
    |
    v
Consumer:

FreeRTOS Task + USB Transmission


# FreeRTOS Integration

The project uses FreeRTOS to handle concurrent execution.

The main synchronization mechanism is a Binary Semaphore



## Binary Semaphore Operation

The DMA ISR gives the semaphore:

```c
xSemaphoreGiveFromISR()
```

The data processing task waits:

```c
xSemaphoreTake()
```

The task remains blocked while no DMA transfer has completed.

The execution flow is:


FreeRTOS Task:

Waiting
   |
   |
   v

DMA finishes buffer

   |
   |
   v

DMA ISR executes

   |
   |
   v

Semaphore released

   |
   |
   v

Task wakes up

   |
   |
   v

Process and transmit data

This avoids polling and unnecessary CPU usage.



# DMA Transfer Counters

Two counters are implemented for system verification.

## dma_transfer_count

Its purpose is to count completed DMA transfers.

The counter is enabled during a controlled measurement window.

A one-minute measurement period is used as a reference to estimate:

Number of completed DMA blocks
Amount of acquired data
Effective acquisition throughput

Example:

Measurement duration:

60 seconds

DMA completed transfers:

N transfers

The counter provides a direct measurement of the amount of data processed by the DMA engine.

## semaphore_overwrite_count

Purpose:

Detects cases where the DMA finishes a new transfer while the previous semaphore notification has not yet been consumed by the FreeRTOS task.

A binary semaphore can store only one pending event:

Semaphore state:

0 -> No event waiting

1 -> Event waiting

If DMA finishes multiple times before the task wakes up:

DMA completion
        |
        v
Semaphore = 1


DMA completion again

        |
        v

Previous notification not consumed

the additional notification cannot be stored.

The counter provides visibility into this condition.


# USB CDC / UART Communication

The communication driver is responsible for transmitting acquired ADC samples from the STM32 to the PC.

The transmission does not occur sample-by-sample.

Instead, data is accumulated in memory buffers.

Example:

ADC Samples:

Sample 1
Sample 2
Sample 3
...
Sample N

are stored inside the DMA buffer.

When the buffer is completed:

DMA Buffer Full

        |

        v

CPU sends complete buffer batch

## Buffer Transmission Size

The DMA buffers contain a fixed number of ADC samples.

Example configuration:

```text
Buffer size:
3000 uint16_t samples

Memory usage:
3000 × 2 bytes = 6000 bytes
```
Specifically, for the three-axis data acquisition:

```text
3000 samples × 2 bytes/sample per X/Y/Z - axis
```
Which entails that the buffer contains 3000 ADC readings distributed among the configured ADC channels.

After DMA completes a buffer transfer, the CPU transmits the completed buffer as a single batch through USB CDC/UART.


## Data Transmission Flow

The complete data path is:

ADC Conversion

        |
        v

DMA Transfer

        |
        v

RAM Buffer

        |
        v

DMA Interrupt

        |
        v

FreeRTOS Semaphore

        |
        v

Processing Task

        |
        v

USB CDC Transmission

        |
        v

PC Application


# Data Integrity Verification

The complete acquisition system was tested experimentally.

The following checks were performed:

Buffer Transmission Verification

The amount of transmitted data was compared against the expected amount generated by DMA.

Results:

Complete buffer contents were transmitted.
Received data size matched the expected transmitted data size.
No observable data loss occurred during the performed tests.
Sampling Timing Verification

The acquired samples were analyzed to verify timing stability.

The measurements confirmed:

Stable sampling intervals.
No unexpected timing gaps.
Continuous ADC acquisition.
Correct DMA synchronization.

The DMA architecture ensures that ADC sampling timing is independent from USB communication timing.

# Performance Advantages

The implemented architecture provides:

CPU Efficiency

The CPU does not continuously monitor ADC conversion status. The FreeRTOS task enters the blocked state until DMA transfer completion occurs


# Stable Sampling

ADC sampling is controlled by hardware peripherals.

DMA transfers occur automatically.

Therefore:

Sampling timing is deterministic.
CPU workload does not affect acquisition frequency.


# Parallel Operation

DMA and CPU operate simultaneously:

DMA:

Acquire new samples


CPU:

Transmit previous samples

This allows continuous acquisition without stopping the ADC.

# Final Architecture Summary

The final system consists of:

- Custom ADC driver
- Custom DMA configuration
- DMA double-buffer memory management
- DMA interrupt handling
- FreeRTOS task synchronization
- Binary semaphore synchronization
- Semaphore overwrite monitoring
- USB CDC/UART data transmission
- Data integrity verification
- ADXL335 analog accelerometer interface

---

# Experimental Results

The system was tested using continuous ADC acquisition through DMA and USB CDC transmission.

The following were verified:

- DMA transfers completed correctly.
- Buffer switching operated correctly.
- FreeRTOS semaphore synchronization worked correctly.
- Complete buffer contents were transmitted.
- Sampling timing remained stable.
- No missing samples were detected by comparing the expected DMA transfer count with the received sample count during the validation period.

The implemented architecture successfully demonstrates a real-time embedded acquisition pipeline using ADC, DMA, interrupts, FreeRTOS synchronization, and USB communication.

## Sampling Rate Verification

The effective acquisition frequency was calculated from the DMA transfer count over a known measurement period.

The measured sampling frequency was approximately:

```text
≈690 samples/second
```

The result confirms stable ADC triggering and DMA transfer operation.



### Benefits of Higher Sampling Frequency

The increased sampling rate enables:

- denser waveform reconstruction  
- improved transient visibility  
- enhanced harmonic analysis  
- more detailed spectral decomposition  
- higher FFT resolution  
- improved bearing fault visibility  

These improvements are critical for predictive maintenance and rotating machinery
diagnostics.

---

## Current Limitation — Telemetry Throughput Bottleneck

The primary bottleneck of the current architecture is **no longer the STM32
acquisition speed**.

Instead, the limiting factor is the transmission of extremely large waveform
datasets through the telemetry pipeline:



### Present Behavior

- waveform data are published **sample-by-sample**  
- FFT spectra are also streamed **point-by-point**  
- MQTT publishing is **synchronous**  
- every measurement triggers an **InfluxDB write operation**  

This results in:

- very large MQTT traffic  
- increased serialization overhead  
- increased database write load  
- slower end-to-end transmission time  

For example, the current implementation intentionally limits transmission to:

```cpp
// Waveform streaming
for (size_t i = 0; i < 2000; i++)

// FFT spectrum streaming
for (size_t i = 0; i < 35000; i++)
````



# Data Structure (version2 branch)

The `version2` branch follows a modular C++ architecture designed for
high‑frequency vibration acquisition, MQTT telemetry, and InfluxDB ingestion.

The directory structure is:

```md
sourceCode/
│
├── conversion/
│   ├── analysis.cpp        # FFT, waveform processing, spectral utilities
│   └── analysis.hpp
│
├── mqtt/
│   ├── mqtt.cpp            # MQTT publisher (sample-by-sample + FFT streaming)
│   └── mqtt.hpp
│
├── influxdbsql/
│   ├── waveform.txt        # Example InfluxDB line protocol for waveform samples
│   └── spectrum.txt        # Example InfluxDB line protocol for FFT bins
│
├── telegraf/
│   └── telegraf.conf.txt   # Telegraf MQTT → InfluxDB ingestion configuration
│
├── config.yaml             # Acquisition, FFT, MQTT, and InfluxDB settings
├── CMakeLists.txt          # Build configuration
└── main.cpp                # End-to-end executable (STM32 → Serial → MQTT → InfluxDB)
```



## What grafana dashboards look like
<img width="1493" height="708" alt="spectra dashboard" src="https://github.com/user-attachments/assets/403fa18b-963a-4830-bb8d-f5f56d3749dd" />
<img width="1495" height="728" alt="waveform dashboard" src="https://github.com/user-attachments/assets/370b277a-c3c4-43ec-afb4-6146f4b50787" />



### Module Responsibilities

**conversion/**
- Implements waveform processing  
- FFT computation  
- spectral feature extraction  
- timestamp reconstruction utilities  

**mqtt/**
- Publishes waveform samples to MQTT  
- Publishes FFT bins  
- Handles synchronous telemetry transmission  
- Implements topic formatting and QoS settings  

**influxdbsql/**
- Contains example line-protocol templates  
- Used for debugging Telegraf → InfluxDB ingestion  

**telegraf/**
- MQTT subscription rules  
- InfluxDB output configuration  
- Measurement naming and tag mapping  

**main.cpp**
- Serial acquisition from STM32  
- Waveform packet decoding  
- FFT execution  
- MQTT publishing loop  
- End-to-end telemetry pipeline  

**config.yaml**
- Sampling frequency  
- FFT window size  
- MQTT broker address  
- InfluxDB bucket/organization  
- Axis selection  
- Acquisition duration

## Running InfluxDB and Telegraf (Administrator PowerShell)

These commands must be executed in an **Administrator PowerShell** because
InfluxDB and Telegraf open network ports and require elevated permissions.

```ps
PS C:\Program Files\InfluxData\telegraf\telegraf-1.38.2> .\telegraf.exe --config "C:\Program Files\InfluxData\telegraf\telegraf.conf"

PS C:\Program Files\InfluxData\influxdb2> .\influxd.exe
```

## Building and Running the C++ Telemetry Pipeline (x64 Native Tools Command Prompt)

The following commands must be executed inside the:

**Start Menu → Developer Command Prompt for VS 2022 → x64 Native Tools**

This environment is required because it:

- loads the MSVC compiler (`cl.exe`)
- loads the MSVC linker
- loads Windows SDK include/lib paths
- loads vcpkg include/lib paths
- ensures correct 64‑bit compilation and linking

### Build the executable

```cmd
C:\Users\arian\Desktop\Vasilisdi-DiplomaUTH_Arduino-Raspberry\sourceCode>cl /EHsc /std:c++17 main.cpp mqtt\mqtt.cpp conversion\analysis.cpp /I C:\vcpkg\installed\x64-windows\include /link /LIBPATH:C:\vcpkg\installed\x64-windows\lib yaml-cpp.lib paho-mqttpp3.lib paho-mqtt3a.lib ws2_32.lib

C:\Users\arian\Desktop\Vasilisdi-DiplomaUTH_Arduino-Raspberry\sourceCode>.\main.exe
```


Future work may contain a machine learning implementation.

## Tags

Project Keywords:

- Arduino
- Raspberry Pi
- STM32
- STM32CubeIDE
- MQTT
- Telegraf
- InfluxDB
- Supabase
- Python
- C++
- Next.js
- Vibration Analysis
- FFT
- Predictive Maintenance
- Industrial IoT
- Vibration Analysis  
- FFT  
- Machine Fault Detection  
- Embedded Systems  
- FreeRTOS  
- DMA  
- ADC  
- Grafana  
- Signal Processing  
- Time‑Series Data  
- Industrial Automation  
- Real‑Time Streaming  
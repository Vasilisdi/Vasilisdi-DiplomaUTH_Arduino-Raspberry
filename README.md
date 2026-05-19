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

# Why STM32 instead of Arduino

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

The system now reaches approximately a range of 7500 Hz of sampling frequency [0-7500 Hz] and this is firmly related to the capacity of stm32, which produces extremely dense vibration waveforms and significantly more accurate frequency-domain analysis. While Arduino may only reach frequency spectra up to 300 Hz. 

However, it became insufficient for:
- improved bearing fault visibility
- clearer harmonic analysis
- higher FFT frequency resolution
- better transient detection


## STM32 Firmware Architecture

The STM32 firmware was developed using:

- STM32CubeMX for peripheral configuration
- STM32CubeIDE for firmware development and debugging

The system uses:

- ADC multi-channel continuous conversion
- DMA-based memory transfers
- USB CDC serial communication
- binary packet streaming

ADC acquisition is initialized through DMA:

```c
HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_LEN);
```

DMA continuously updates the ADC buffer:

```text
adc_buffer[0] -> X axis
adc_buffer[1] -> Y axis
adc_buffer[2] -> Z axis
```

The firmware creates atomic snapshots of the ADC values:

```c
__disable_irq();
x = adc_buffer[0];
y = adc_buffer[1];
z = adc_buffer[2];
__enable_irq();
```

Each ADC value is transmitted as a 16-bit binary value:

```c
packet[i++] = x >> 8;
packet[i++] = x & 0xFF;
```

Each waveform sample contains:

```text
X axis -> 2 bytes
Y axis -> 2 bytes
Z axis -> 2 bytes
```

Resulting in:

```text
6 bytes per sample
```

Multiple samples are aggregated into larger USB packets:

```c
#define SAMPLES_PER_PACKET 10
#define PACKET_SIZE (SAMPLES_PER_PACKET * 6)
```

This produces:

```text
10 samples × 6 bytes = 60-byte binary packets
```

On the PC side, the C++ application reconstructs the original ADC values:

```cpp
uint16_t x = (buffer[i*6] << 8) | buffer[i*6+1];
uint16_t y = (buffer[i*6+2] << 8) | buffer[i*6+3];
uint16_t z = (buffer[i*6+4] << 8) | buffer[i*6+5];
```

## High-Frequency Acquisition Performance (version2 branch)

The STM32-based architecture achieves significantly higher sampling throughput
compared to the original Raspberry Pi implementation.

Typical performance:

- ~450,000 samples per minute, with 7500 Hz range

In certain experimental configurations, the system approaches:

- ~900,000 samples per minute

The exact throughput depends on:

- FFT window size  
- acquisition duration  
- serial transfer stability  
- MQTT throughput  
- InfluxDB ingestion rate  
- waveform packet size  
- Grafana refresh intervals  

This represents a major improvement over the original Raspberry Pi design and
demonstrates the substantially higher throughput capabilities of the STM32-based
real-time acquisition pipeline.

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

## Project Keywords

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

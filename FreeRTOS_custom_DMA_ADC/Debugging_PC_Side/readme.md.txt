# STM32 ADC DMA FreeRTOS Data Acquisition System

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

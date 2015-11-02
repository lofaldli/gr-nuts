# Design review of the NUTS Ground Station

## Requirements table

| Code           | Description                                            |
| -------------- | ------------------------------------------------------ |
| R05-GS-NGH-001 | The ground station must send commands to the satellite |
| R02-GS-CW-001  | The ground station must decode the beacon signal       |
| R03-GS-NGH-002 | The ground station must process housekeeping data      |
| R04-GS-NGH-003 | The ground station must receive payload data from the  |
|                | satellite                                              |
| R06-GS-NGH-004 | The ground station must use FEC to prevent data        |
|                | corruption and loss                                    |

## System overview

The role of the ground station is to provide communication with the satellite for the duration of the mission. The communtication will take place on two different frequencies, VHF (145.98 MHz) and UHF (437.305 MHz), to give some flexibility with regards to noise and other communication. The data is encoded using the [NGHAM](https://github.com/skagmo/ngham) packet radio protocol developed by J. Skagmo, LA3JPA. 

The RF-frontend consists of an antenna with a PA for uplink and an LNA for downlink. This is connected to an USRP which performs filtering and mixing for basisband processing. The basisband signal processing and encoding/decoding is done by a computer running GNURadio. 


## GNU Radio (v3.7.9)

GNU Radio is a free software deeltopment toolkit tha tprovides signal procesing blocks to implement SDRs and signal processing systems. It communicates with the hardware over ethernet using raw I/Q samples. These samples are processed using module blocks written in either python or c++, and converted to and from a meaningful bitstream of data. 

These blocks include:

- GMSK Mod/Demod: Converts between I/Q symbols and bits
- NGHAM Encode/Decode: Converts between data bits and encoded data bits, the encoded bits include the data bits as well as CRC bits and Reed-Solomon bits used for Forward Error Correction

## USRP

The USRP is a Software Defined Radio meaning that the usual components of a radio (mixers, filters, amps, etc.) are all implemented in software. The USRP has an FPGA which converts real analog signals to complex baseband signals in the digital domain. These are transferred to the host computer running GNU Radio via an ethernet cable.

It should have a frontend allowing multiple antenna inputs for circularly polarized signals.

## RF-Frontend

- LNA/PA
- Antenna: circultarly polarized

## Gpredict

used for calculating pass times and positions to control the antenna, also doppler shift which must be corrected by the ground station (GNU Radio?)

## Abbreviations

- CW: Continous wave
- FEC: Forward Error Correction
- GS: Ground Station
- I/Q: In-phase/Quadrature
- LNA: Low Noise Amplifier
- NGH/NGHAM: Next Generation Ham
- PA: Power Amplifier
- RF: Radio Frequency
- RS: Reed-Solomon
- SDR: Software Defined Radio
- UHF: Ultra high frequency
- USRP: Universal Soft Radio Peripheral
- VHF: Very high frequency

# Design review of the NUTS Ground Station

## Requirements table

| Code           | Description                                            |
| -------------- | ------------------------------------------------------ |
| R05-GS-NGH-001 | The ground station must send commands to the satellite |
| R02-GS-CW-001  | The ground station must decode the beacon signal       |
| R03-GS-NGH-002 | The ground station must process housekeeping data      |
| R04-GS-NGH-003 | The ground station must receive payload data from the  |
|                | satellite                                              |
| R06-GS-RS-001  | The ground station must use FEC to prevent data        |
|                | corruption and loss                                    |

## System overview

The role of the ground station is to provide communication with the satellite for the duration of the mission. The communtication will take place on two different frequencies, VHF (145.98 MHz) and UHF (437.305 MHz), to give some flexibility with regards to noise and other communication. The data is encoded using the [NGHAM](https://github.com/skagmo/ngham) packet radio protocol developed by J. Skagmo, LA3JPA. 

The RF-frontend consists of an antenna with a PA for uplink and an LNA for downlink. This is connected to an USRP which performs filtering and mixing for basisband processing. The basisband signal processing and encoding/decoding is done by a computer running GNURadio. 


## GNURadio



## USRP

## RF-Frontend

- LNA/PA
- Antenna: circultarly polarized

## Gpredict

...

## Flyby

...

## Abbreviations

- CW: Continous wave
- GS: Ground Station
- LNA: Low Noise Amplifier
- NGH/NGHAM: Next Generation Ham
- PA: Power Amplifier
- RF: Radio Frequency
- RS: Reed-Solomon
- SDR: Software Defined Radio
- UHF: Ultra high frequency
- USRP: Universal Soft Radio Peripheral
- VHF: Very high frequency

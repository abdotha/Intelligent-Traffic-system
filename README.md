# Traffic Control System with Microcontrollers

## Overview
This project implements a traffic control system for both cars and pedestrians. When a pedestrian pushes a button, the system interrupts the regular traffic flow to allow them to cross safely, then returns to its normal operation. The project utilizes two microcontrollers communicating via UART and incorporates components such as seven-segment displays and LCDs for pedestrian information, as well as LEDs and servo motors to control pedestrian access.

## Features
- Interrupt-based pedestrian crossing system
- Communication between two microcontrollers via UART
- Seven-segment displays for vehicle signals
- LCD display for pedestrian information
- LEDs to indicate pedestrian crossing status
- Servo motor to control pedestrian access

## Components
- Two microcontrollers ( 89s52 & Atmega 16 )
- Seven-segment displays
- LCD display
- LEDs
- Servo motor

## Implementation
The project is implemented in the C programming language. Each microcontroller is responsible for controlling specific aspects of the traffic system, such as vehicle signals or pedestrian access. Communication between the microcontrollers is achieved through UART.

## Usage
1. Connect the microcontrollers and all components as per the circuit diagram provided (if available).
2. Upload the respective firmware to each microcontroller.
3. Power on the system and observe the traffic flow.
4. When a pedestrian wishes to cross, they should push the designated button.
5. The system will interrupt the traffic flow, allowing the pedestrian to cross safely.
6. After a specified duration or when the pedestrian has crossed, the system will return to normal traffic operation.

## Circuit Diagram
(Included in the repo)


# HSR-8498HB servo driver

This is a rudimentary driver for the HSR-8498HB serial interface servo motor.

The serial driver itself is taken from an example I found at mikrocontroller.net and modified.

This was originally used whit the odroid single-chip computer but should work on any machine
that runs Linux and has a serial port device that is connected to the motor.

The driver is provided as-is, under GPL v3. No support given since I do not have such motors
to test anything with.

# Connection

If I remember correctly, the motors have only a single serial connection line which is used for
Tx and Rx simultaneously. The motor "answers" within a certain byte slot coherent with the command.

Hence, connect Rx and Tx of your USB-TTL-adaptor (or whatever you use in that case) both to the
serial com's line of the motor.

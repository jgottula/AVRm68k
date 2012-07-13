#!/bin/bash

sudo rmmod -f ftdi_sio
sudo avrdude -p m1284p -c usbasp -B 460800 -t
sudo modprobe ftdi_sio

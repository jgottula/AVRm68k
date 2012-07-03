#!/bin/bash

#sudo modprobe -r ftdi_sio
sudo avrdude -p m324a -c usbasp -t
#sudo modprobe ftdi_sio

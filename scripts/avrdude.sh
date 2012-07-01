#!/bin/bash

#sudo modprobe -r ftdi_sio
sudo avrdude -p m168 -c usbasp -t
#sudo modprobe ftdi_sio

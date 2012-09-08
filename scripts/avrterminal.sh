# AVRm68k
# (c) 2012 Justin Gottula
# The source code of this project is distributed under the terms of the
# simplified BSD license. See the LICENSE file for details.

# avrdude terminal script

sudo rmmod -f ftdi_sio
sudo avrdude -p m1284p -c usbasp -B 230400 -t
sudo modprobe ftdi_sio

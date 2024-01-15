#!/bin/sh

picotool load $1/picoled.uf2 -F

sleep 1

picotool reboot
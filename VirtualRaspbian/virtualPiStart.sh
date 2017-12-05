#!/bin/sh

# Check if there were two arguments given to the script
if [ $# -eq 2 ]
	then
	qemu-system-arm -kernel $1 -cpu arm1176 -m 256 -M versatilepb -serial stdio -append "root=/dev/sda2 rootfstype=ext4 rw" -hda $2 -redir tcp:5022::22 -no-reboot
else
	echo "Usage: <script name> <kernel-file> <image-file>"
fi
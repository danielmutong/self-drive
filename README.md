# self-drive

The goal of this project is to create a self-driving car algorithm that can be run on an Arm microcontroller running Linux and an FPGA.
The FPGA receives a video stream via HDMI and after reducing the image size and turning it into greyscale, 
is passed onto an Arm microcontroller to perform lane detection and self-driving, and potentially, object detection.

Currently, the lane detection and self-driving is complete and is now in progress of integrating with the firmware.

# self-drive

The goal of this project is to create a self-driving car algorithm that can be run on an ARM microcontroller running Linux and an FPGA.
The FPGA receives a video stream via HDMI and after reducing the image size and turning it into greyscale, 
is passed onto an Arm microcontroller to perform lane detection and self-driving, and potentially, object detection.

Currently, the lane detection and self-driving is complete and is now in progress of integrating with the firmware.
Object detection has been experimented with. However, the resulting product runs too slowly to be integrated with the software.
Further details are explained within their corresponding folders.

Todo List:

Software:

Train OCR to recognize digits on speedometer for use to improve driving speed
Experiment with Blob Detection to use as an subsitute for ML object detection.

Hardware:

Firmware:

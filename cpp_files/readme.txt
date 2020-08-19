This self-driving and lane detection algorithms are built using OpenCV for C++. 

Breakdown:
1) The game window is found with the help of Xdo library
2) Each frame is passed as a Mat file and initally resized and turned into grayscale
3) A region of interest (the road) is created and that part of the frame undergoes Canny edge detection to find the edges and then goes through Probabilistic Hough Line Transform (PHLT) which returns all the lines found in the region of interest
4) Since PHLT returns many small lines that make up bigger curves and lines, the most difficult part of the project was to combine these lines based on slope, y-intercept, and distance from each other. 
5) Once the lines are combined, the closest right line and the closest left line to the centre of the screen at five different y-coordinates are extracted, resulting in ten lines, five on the right and five on the lieft
6) From the ten lines, the exact x-coordinates that corresponds to each of the five y-coordinates are calculated and then connected to produce one line on the left and one line on the right. These two lines are then used as the right boundary of the road and the left boundary of the road
7) Based on points from the left bound and right bound, an offset is calculated from the difference between the right bound and the left bound. This offset is then passed into a PID controller which uses generated keystrokes to correct the direction of the car based on the offset



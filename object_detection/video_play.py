import numpy as np
import cv2

cap = cv2.VideoCapture('cars.mp4')

while(cap.isOpened()):
    ret, frame = cap.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    cv2.rectangle(gray,(100, 100), (200,200), color = (0,255,0), thickness = 1)

    cv2.imshow('frame',gray)
    cv2.waitKey(30)

cap.release()
cv2.destroyAllWindows()

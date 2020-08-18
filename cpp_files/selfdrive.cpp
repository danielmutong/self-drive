#include "selfdrive.hpp"
#include "uhidkb.h"
#include "usbkeycode.h"
#include "project.hpp"

float selfdrive::self_drive_control(single_line left_bound[], single_line right_bound[], float lasterr)
{
  
  float kp = 2;
  float kd = 0;
  float ki = 1;
  float offset;
  
  int offsetleft = (300 - left_bound[1].x0 );
  int offsetright = (right_bound[1].x0 - 300);  
  offset = offsetleft - offsetright;
  int set = 0;
  int error = abs(set - offset);
  float p = kp * error;
  float d = kd * (error - lasterr);
  float i = ki * error + i;
  int g = p + i + d;
  int k = g * 0.1;
  // uhid_key_event(pfds.fd, USB_KEY_W,  50);
  cout << "test point" << offset << " " << left_bound[1].x0 << " " << right_bound[1].x0  << endl;
  uhid_key_event(pfds.fd, USB_KEY_W, 20);

  if (abs(offset) < 100)
    uhid_key_event(pfds.fd, USB_KEY_W, 50);

  if ((g<1000) && (k < 1000)){
    if(offset < 0){
       uhid_key_event(pfds.fd, USB_KEY_D, abs(g));
       uhid_key_event(pfds.fd, USB_KEY_S, abs(k));

    }
  
    else if(offset > 0){
       uhid_key_event(pfds.fd, USB_KEY_A, abs(g));
       uhid_key_event(pfds.fd, USB_KEY_S, abs(k));

    }
  }
  
  
  lasterr = error;
  cout << "last error: " << lasterr << endl;
  return lasterr;
}

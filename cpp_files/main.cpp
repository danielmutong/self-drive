#include "analysis.hpp"
#include "project.hpp"
#include "road.hpp"
#include "selfdrive.hpp"
#include "single_line.hpp"
#include "uhidkb.h"
#include "operation.cpp"

int main(int argc, char* argv[])
{
  
  cv::String window_name = "My First Video";
  Display *disp = XOpenDisplay (NULL);
  uhid_init();
  namedWindow(window_name, WINDOW_NORMAL); 
  ScreenShot screen("torcs", 0,0,640,480);

  while (true)
    {
      Mat frame, a;
      int merged_lines_size;
      road roadanalysis;
      Mat img;
      single_line merged_lines[500];
      selfdrive test1;

      screen(a);
      resize(a, img, Size(600,400));

      roadanalysis.detect_road(img, merged_lines);

      float lasterr = 0; 
      lasterr = test1.self_drive_control(roadanalysis.left_bounds, roadanalysis.right_bounds, lasterr);


      waitKey(20);
    }
  uhid_stop(pfds.fd);
  return 0;

}

#ifndef _selfdrive_hpp_
#define _selfdrive_hpp_
#include "single_line.hpp"

class selfdrive
{
  
public:
  float difference[2];
  float right[2],left[2];
  float self_drive_control(single_line [], single_line [], float);
  void key_press(void);
  
};

#endif

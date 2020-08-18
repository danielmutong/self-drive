#ifndef _single_line_hpp_
#define _single_line_hpp_
#include "project.hpp"

class single_line
{
  
public:
  int x0;
  int x1;
  int y0;
  int y1;
  float m;
  float y_int;
  bool mergeflag = false;
  
  void find_slope(Vec4i);
  void output(void);
  bool merge(single_line otherline, float b_thres, float m_thres, float d_thres, float max_dist);
  double distance_calc_min(single_line otherline );
  double distance_calc_max(single_line otherline);  
  void find_slope_second_round(Vec4i l);

};

#endif

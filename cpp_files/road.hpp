#ifndef _road_hpp_
#define _road_hpp_
#include "project.hpp"
#include "single_line.hpp"

class road
{

public:
  Mat img;
  Mat roi;
  Mat original;
  int i=0;
  int k = 0;

  int merged_lines_size;
  single_line all_merged_lines [500];
  single_line final_y [50][500];
  vector <Vec4i> temp_left_bounds, temp_right_bounds;
  single_line left_bounds [20];
  single_line right_bounds [20];
  int final_size[50] = {0} ;
  int count   =  0;
  float left_bound[20], right_bound[20];
  
  void get_x_coord(void);
  void sort_x(void);
  void get_bounds(void);
  int y_val=250;
  float x_coord[50][200];
  single_line lines[500];
  void detect_road(cv::Mat, single_line []);
  void getslopes(vector <Vec4i>);
  void connect_bounds(void);

};

#endif

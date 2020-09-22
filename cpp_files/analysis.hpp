#ifndef _analysis_hpp_
#define _analysis_hpp_

#include "project.hpp"
#include "single_line.hpp"
class analysis
{
public:
  analysis(){};
  ~analysis(){};
  int control_ana( std::vector<cv::Vec4i> ,float , float , float , float , Mat , single_line  [] );
  int merge_line(single_line [], int ,float, float , float , float , single_line [], Mat );
  void sort_slopes(single_line [], int );

};

#endif

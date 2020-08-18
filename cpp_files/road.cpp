
#include "road.hpp"
#include "analysis.hpp"

void road::detect_road(cv::Mat src, single_line merged_lines [])
{
  
  float m_thres = M_THRES;
  float b_thres = B_THRES;
  float d_thres = D_THRES;
  float max_dist=0;
  Mat original2;

  Mat dst, cdstP, cdst;
  Mat src1, src2, mask, fin_src;
  cv::cvtColor(src,src2, cv::COLOR_BGR2HSV);
  cv::cvtColor(src,src1,cv::COLOR_BGR2GRAY);

  img = src;
  original = img.clone();
  original2 = img.clone();
  Rect lane(10,200,580,90);
  Mat srclane = img(lane);
  imshow("srclane", srclane);

  // Edge detection
  Canny(srclane, dst, 100,200, 3);  
  cvtColor(dst, cdst, COLOR_GRAY2BGR); 
  cdstP = cdst.clone();

  // Probabilistic Line Transform
  vector<Vec4i> linesP; 
  HoughLinesP(dst, linesP, 1, CV_PI/180, 20,10, 10 );
  
  for( size_t i = 0; i < linesP.size(); i++ )
    {
      Vec4i l = linesP[i];
      line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, LINE_AA);
      line( original, Point(l[0]+10, l[1]+200), Point(l[2]+10, l[3]+200), Scalar(0,0,255), 1, LINE_AA);
    }

  imshow( "original " , original);

  analysis test;
  merged_lines_size=test.control_ana(linesP, m_thres, b_thres, d_thres,max_dist, img, merged_lines);

  for (int i = 0 ; i < merged_lines_size ; i++)
    {
      all_merged_lines[i] = merged_lines[i];
    }
  
  y_val = 220;
  
  for (y_val = 220; y_val <251; y_val += 5)
    {
      get_x_coord();
      count++;
    }

  count = 0;
  
  for (count = 0; count < 5; count++)
    {
      sort_x();
    }

  for (count = 0; count < 5; count++)
    {
      get_bounds();    
    }

  int j = 0;
  
  for (y_val = 220; y_val <251; y_val += 5)
    {

      char c[10];
      char c1[10];
      snprintf(c,10,"%d",(int) left_bound[j]);
      snprintf(c1,10,"%d",(int) right_bound[j]);

      putText(img, c, cv::Point(left_bound[j],y_val),  FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,250), 1, LINE_AA);
      putText(img, c1, cv::Point(right_bound[j],y_val),  FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,250), 1, LINE_AA);

      j++;
    }

  connect_bounds();

  for( size_t i = 0; i < 4; i++ )
    {
      line( original2, Point(left_bounds[i].x0, left_bounds[i].y0), Point(left_bounds[i].x1, left_bounds[i].y1), Scalar(0,0,255), 1, LINE_AA);
      line( original2, Point(right_bounds[i].x0, right_bounds[i].y0), Point(right_bounds[i].x1, right_bounds[i].y1), Scalar(0,0,255), 1, LINE_AA);
    }

  imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
  getslopes(linesP);

  imshow("original", img);
  imshow("bounds", original2);

}


void road::getslopes(vector<Vec4i> lines)
{

  float m[lines.size()];
  float y_int[lines.size()];

  for (int i = 0; i < lines.size(); i++)
    {
      Vec4i l = lines[i];
      m[i] = (float)(l[3] - l[2]) / (l[1] - l[0]);
      y_int[i]=l[2] - m[i] * l[0];
    }

}


void road::get_x_coord(void)
{
  
  int j = 0;
  
  for (int i = 0; i < merged_lines_size ; i++){
    
    if (((all_merged_lines[i].y0  ) >  y_val ) && ((all_merged_lines[i].y1 )  < y_val ))
      {
        x_coord[count][j] = (y_val - all_merged_lines[i].y_int) / all_merged_lines[i].m;
        final_y[count][j] = all_merged_lines[i];
        j++;
      }
    
  }
  final_size[count] = j;
}


void road::sort_x(void)
{
  
  for(int i = 0; i < final_size[count]; i++)
    {		
      for(int j = i + 1; j < final_size[count]; j++)
        {
          if(x_coord[count][i] > x_coord[count][j])
            {
              float temp = x_coord[count][i];
              x_coord[count][i] = x_coord[count][j];
              x_coord[count][j] = temp;
            }
        }
    }
}


void road::get_bounds(void)
{

  if (final_size[count] == 0)
    {
      right_bound[count] = 600;
      left_bound[count] = 0;
      k = 1;
    }
  else{
    
    for(int i = 0; i < final_size[count]; i++)
      {

        if (x_coord[count][i] > 300)
          {
            k = 2;
            right_bound[count] = x_coord[count][i];
            left_bound[count] = x_coord[count][i-1];
            break;
          }
      
        else if (x_coord[count][0] > 300)
          {
            k = 3;
            right_bound[count] = x_coord[count][0];
            left_bound[count] = 0;
          }
      
        else if (x_coord[count][final_size[count]-1] < 300)
          {
            k = 4;
            right_bound[count] = 600;
            left_bound[count] = x_coord[count][final_size[count]-1];     
          }
      
        else
          {
            k = 5;
            right_bound[count] = 600;
            left_bound[count] = 0;
     
          }
      }
  }  
}


void road::connect_bounds(void)
{
 
  int y_val = 220;
  
  for ( int i = 0; i < 4; i ++)
    {
      Vec4i l;
      Vec4i r;
      l[0] = left_bound[i];
      l[2] = left_bound[i + 1];
      l[1] = y_val;
      l[3] = y_val + 5;
      left_bounds[i].find_slope_second_round(l);

      r[0] = right_bound[i];
      r[2] = right_bound[i + 1];
      r[1] = y_val;
      r[3] = y_val + 5;
      right_bounds[i].find_slope_second_round(r);

      y_val += 5;    
    }
}
  


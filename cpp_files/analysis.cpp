#include "analysis.hpp"

int analysis::control_ana( std::vector<cv::Vec4i> lines,float m_thres, float b_thres, float d_thres, float max_dist, Mat a, single_line merged_lines [] ){
  
  single_line all_lines[1000];
  int original_number_of_lines = 0;
  for (int i = 0; i < lines.size(); i++)
    {
      Vec4i l = lines[i];
      all_lines[i].find_slope(l);
      original_number_of_lines++;
    }
  
  for(int i = 0; i < original_number_of_lines; i++ )
    {
      sort_slopes(all_lines, original_number_of_lines);
    }
  
  single_line new_line[256];
  
  int new_line_count=0;
  int roundcnt = 0;

  single_line round1merge_arr[500];
  single_line round2merge_arr[500];
  int round1merge_num = merge_line(all_lines, original_number_of_lines, b_thres,m_thres, d_thres, max_dist, round1merge_arr,a);
  int round2merge_num = merge_line(round1merge_arr, round1merge_num,b_thres,m_thres, d_thres, max_dist, round2merge_arr,a);

  vector<Vec4i> line_final;
  
  for(int i = 0; i < round2merge_num; i++ )
    {
      cv::line( a, Point(round2merge_arr[i].x0  , round2merge_arr[i].y0 ), Point(round2merge_arr[i].x1 ,round2merge_arr[i].y1 ), Scalar(0,0,255), 1, LINE_AA);
    }

  imshow("merged",a);
 
  for (int i = 0 ; i < round2merge_num ; i++)
    {
      merged_lines[i] = round2merge_arr[i];
    }
  
  return round2merge_num;

}



void analysis::sort_slopes(single_line all_lines[], int original_number_of_lines){
    
  for (int i = 0; i < original_number_of_lines; i++)
    {
      for (int j = i + 1; j < original_number_of_lines; j++)
        {
          if (all_lines[i].m > all_lines[j].m) 
            {
              single_line a=  all_lines[i];
              all_lines[i] = all_lines[j];
              all_lines[j] = a;
            }
        }
    }
  
}


int analysis::merge_line(single_line all_lines[], int num_of_lines,float b_thres, float m_thres, float d_thres, float max_dist, single_line new_line[], Mat a){

  int num_of_new_lines;

  for(int i = 0; i < num_of_lines-1; i++)
    {
      if (all_lines[i].mergeflag == false)
        {
          for (int j = i+1; j < num_of_lines; j++)
            {
              if (all_lines[j].mergeflag == false)
                {
                  if (all_lines[i].merge(all_lines[j], b_thres, m_thres, d_thres, max_dist))
                    {
                      all_lines[j].mergeflag = true;
                    }
                  num_of_new_lines++;
                }
            }
        }
    }
  int new_line_size = 0;
  
  for(int i = 0; i < num_of_lines; i++)
    {
      if (all_lines[i].mergeflag == false)
        {
          new_line[new_line_size] = all_lines[i];
          new_line_size++;
        }
    }
  
  return new_line_size;
  
}


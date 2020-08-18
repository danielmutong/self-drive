#include "single_line.hpp"

void single_line::find_slope(Vec4i l)
{

  if(l[1] < l[3])
    {
      int temp_y = l[1];
      l[1] = l[3];
      l[3] = temp_y;
      int temp_x = l[0];
      l[0] = l[2];
      l[2] = temp_x;
    }
    
  x0 = l[0] + 10;
  x1 = l[2] + 10;
  y0 = l[1] + 200;
  y1 = l[3] + 200;
  m = (float)(y1 - y0) / (x1 - x0);
  y_int=y0-m*x0;
  
}


void single_line::find_slope_second_round(Vec4i l)
{

  if(l[1] < l[3])
    {
      int temp_y = l[1];
      l[1] = l[3];
      l[3] = temp_y;
      int temp_x = l[0];
      l[0] = l[2];
      l[2] = temp_x;
    }
    
  x0 = l[0];
  x1 = l[2];
  y0 = l[1];
  y1 = l[3];
  m = (float)(y1 - y0) / (x1 - x0);
  y_int=y0-m*x0;
  
}


void single_line::output(void)
{
  cout<< x0<<" "<<y0<<" "<<x1<<" "<<y1<<" "<<m<<" "<<y_int<<endl;
}


bool single_line::merge(single_line b, float b_thres, float m_thres, float d_thres, float max_dist)
{
  if ((abs(m - b.m) < m_thres)
      && (abs(y_int - b.y_int) < b_thres)
      && (this->distance_calc_min(b) < d_thres))
    {      
      float max = distance_calc_max(b);
      return true;
    }

  return false;
    
}


double single_line:: distance_calc_min(single_line b)
{
  
  double dist[4];
  dist[0] = pow((x0 - b.x0), 2) + pow((y0 - b.y0), 2);
  dist[1] = pow((x0 - b.x1), 2) + pow((y0 - b.y1), 2);
  dist[2] = pow((x1 - b.x0), 2) + pow((y1 - b.y0), 2);
  dist[3] = pow((x1 - b.x1), 2) + pow((y1 - b.y1), 2);

  double min=dist[0];

  for(int i=1; i < 4; i++)
    {
      if (dist[i] < min)
	{
	  min = dist[i];
	}
    }

  return min;
  
}


double single_line:: distance_calc_max(single_line b)
{
  
  double dist[6];
  dist[0] = pow((x0 - b.x0), 2) + pow((y0 - b.y0), 2);
  dist[1] = pow((x0 - b.x1), 2) + pow((y0 - b.y1), 2); 
  dist[2] = pow((x1 - b.x0), 2) + pow((y1 - b.y0), 2);
  dist[3] = pow((x1 - b.x1), 2) + pow((y1 - b.y1), 2);
  dist[4] = pow((x0 - x1), 2) + pow((y0-y1),2);
  dist[5] = pow((b.x0 - b.x1), 2) + pow((b.y0-b.y1),2);

  int j = 0;
  double max = dist[0];

  for(int i = 1; i < 6; i++)
    {
      if (dist[i] > max)
        {
          max = dist[i];
          j=i;
        }
    }

  switch(j){
    
  case 0:
    
    if(y0 > b.y0)
      {
        y0 = y0;
        y1 = b.y0;
        x0 = x0;
        x1 = b.x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int=y0-m*x0;
      }
    
    else
      {
        y1 = y0;
        y0 = b.y0;
        x1 = x0;
        x0 = b.x0;
     
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;

      }
    break;
    
  case 1:
    
    if(y0 > b.y1)
      {
        y0 = y0;
        y1 = b.y1;
        x0 = x0;
        x1 = b.x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    
    else
      {
        y1=y0;
        y0=b.y1;
        x1=x0;
        x0=b.x1;
    
        m=(float)(y1-y0)/(x1-x0);
        y_int=y0-m*x0;
      }
    break;
    
  case 2:
    
    if(y1>b.y0)
      {
        y0=y1;
        y1=b.y0;
        x0=x1;
        x1=b.x0;
        m=(float)(y1-y0)/(x1-x0);
        y_int=y0-m*x0;
      }
    
    else
      {
        y0 = b.y0;
        y1 = y1;
        x0 = b.x0;
        x1 = x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    break;
    
  case 3:
    
    if(y1 > b.y1)
      {
        y0 = y1;
        y1 = b.y1;
        x0 = x1;
        x1 = b.x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    
    else
      {
        y0 = b.y1;
        y1 = y1;
        x0 = b.x1;
        x1 = x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    break;
    
  case 4:
    
    if (y0 > y1)
      {
        y0 = y0;
        y1 = y1;
        x0 = x0;
        x1 = x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    
    else
      {
        int temp = y1;
        int temp2 = x1;
        y1 = y0;
        y0 = temp;
        x1 = x0;
        x0 = temp2;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    break;

  case 5:
    
    if (b.y0 > b.y1)
      {
        y0 = b.y0;
        y1 = b.y1;
        x0 = b.x0;
        x1 = b.x1;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    
    else
      {
        int temp = b.y1;
        int temp2 = b.x1;
        y1 = b.y0;
        y0 = temp;
        x1 = b.x0;
        x0 = temp2;
        m = (float)(y1 - y0) / (x1 - x0);
        y_int = y0 - m * x0;
      }
    break;
  }
  
  return dist[j];

}



#ifndef _project_hpp_
#define _project_hpp_
#define POSY0 175
#define YCOORD 240
#define MIDPOINT 300
#define M_THRES 0.2
#define B_THRES 10
#define D_THRES 5000

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XTest.h>
#include <math.h>
#include "uhidkb.h"
extern "C"{
#include <unistd.h>
#include <xdo.h>
}

using namespace cv;
using namespace std;

#endif

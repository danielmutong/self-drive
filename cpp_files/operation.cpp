#include "project.hpp"

struct ScreenShot
{
  ScreenShot(const char* name, int x, int y, int width, int height):
    x(x),
    y(y),
    width(width),
    height(height)
  {

    display = XOpenDisplay(nullptr);
    Window *list;
    xdo_search_t search;
    unsigned int nwindows;
    memset(&search, 0, sizeof(xdo_search_t));
    search.max_depth = -1;
    search.require = xdo_search::SEARCH_ANY;

    search.searchmask |= SEARCH_NAME;
    search.winname = name;

    xdo_t* p_xdo = xdo_new(NULL);
    int id = xdo_search_windows(p_xdo, &search, &list, &nwindows);

    if (list == NULL) {
      cout << " not found " << endl;
    } else
      cout << " total =  " << nwindows << endl;
    root = list[0];
    init = true;
  }

  void operator() (Mat& cvImg)
  {
    if(init == true)
      init = false;
    else
      XDestroyImage(img);

    img = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap);

    cvImg = Mat(height, width, CV_8UC4, img->data);
  }

  ~ScreenShot()
  {
    if(init == false)
      XDestroyImage(img);

    XCloseDisplay(display);
  }

  Display* display;
  Window root;
  int x,y,width,height;
  XImage* img;

  bool init;
};


static void SendKey (Display * disp, KeySym keysym, KeySym modsym)
{

  KeyCode keycode = 0, modcode = 0;
  keycode = XKeysymToKeycode (disp, keysym);
  if (keycode == 0) return;
  XTestGrabControl (disp, True);
  /* Generate modkey press */

  if (modsym != 0)
    {
      modcode = XKeysymToKeycode(disp, modsym);
      XTestFakeKeyEvent (disp, modcode, True, 0);
    }
  /* Generate regular key press and release */
  XTestFakeKeyEvent (disp, keycode, True, 50);
  XTestFakeKeyEvent (disp, keycode, False, 0);

  /* Generate modkey release */
  if (modsym != 0)
    XTestFakeKeyEvent (disp, modcode, False, 0);

  XSync (disp, False);
  XTestGrabControl (disp, False);
  
}

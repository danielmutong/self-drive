#ifndef _UHIDKB_H_
#define _UHIDKB_H_
#include <poll.h>

int uhid_init(void);
void uhid_stop(int fd);
int uhid_key_event(int fd, char key, unsigned int holdtime);
extern struct pollfd pfds;

#endif


  

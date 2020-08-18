//#define _CFG_DEBUG_UHID_

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <linux/uhid.h>
#include "usbkeycode.h"

/*
 * HID Report Desciptor
 * We emulate a basic 3 button mouse with wheel and 3 keyboard LEDs. This is
 * the report-descriptor as the kernel will parse it:
 *
 * INPUT(1)[INPUT]
 *   Field(0)
 *     Physical(GenericDesktop.Pointer)
 *     Application(GenericDesktop.Mouse)
 *     Usage(3)
 *       Button.0001
 *       Button.0002
 *       Button.0003
 *     Logical Minimum(0)
 *     Logical Maximum(1)
 *     Report Size(1)
 *     Report Count(3)
 *     Report Offset(0)
 *     Flags( Variable Absolute )
 *   Field(1)
 *     Physical(GenericDesktop.Pointer)
 *     Application(GenericDesktop.Mouse)
 *     Usage(3)
 *       GenericDesktop.X
 *       GenericDesktop.Y
 *       GenericDesktop.Wheel
 *     Logical Minimum(-128)
 *     Logical Maximum(127)
 *     Report Size(8)
 *     Report Count(3)
 *     Report Offset(8)
 *     Flags( Variable Relative )
 * OUTPUT(2)[OUTPUT]
 *   Field(0)
 *     Application(GenericDesktop.Keyboard)
 *     Usage(3)
 *       LED.NumLock
 *       LED.CapsLock
 *       LED.ScrollLock
 *     Logical Minimum(0)
 *     Logical Maximum(1)
 *     Report Size(1)
 *     Report Count(3)
 *     Report Offset(0)
 *     Flags( Variable Absolute )
 *
 * This is the mapping that we expect:
 *   Button.0001 ---> Key.LeftBtn
 *   Button.0002 ---> Key.RightBtn
 *   Button.0003 ---> Key.MiddleBtn
 *   GenericDesktop.X ---> Relative.X
 *   GenericDesktop.Y ---> Relative.Y
 *   GenericDesktop.Wheel ---> Relative.Wheel
 *   LED.NumLock ---> LED.NumLock
 *   LED.CapsLock ---> LED.CapsLock
 *   LED.ScrollLock ---> LED.ScrollLock
 *
 * This information can be verified by reading /sys/kernel/debug/hid/<dev>/rdesc
 * This file should print the same information as showed above.
 */

static unsigned char rdesc[] = {
				0x05, 0x01,                         // Usage Page (Generic Desktop)
				0x09, 0x06,                         // Usage (Keyboard)
				0xA1, 0x01,                         // Collection (Application)
				0x05, 0x07,                         //     Usage Page (Key Codes)
				0x19, 0xe0,                         //     Usage Minimum (224)
				0x29, 0xe7,                         //     Usage Maximum (231)
				0x15, 0x00,                         //     Logical Minimum (0)
				0x25, 0x01,                         //     Logical Maximum (1)
				0x75, 0x01,                         //     Report Size (1)
				0x95, 0x08,                         //     Report Count (8)
				0x81, 0x02,                         //     Input (Data, Variable, Absolute)

				0x95, 0x01,                         //     Report Count (1)
				0x75, 0x08,                         //     Report Size (8)
				0x81, 0x01,                         //     Input (Constant) reserved byte(1)

				0x95, 0x05,                         //     Report Count (5)
				0x75, 0x01,                         //     Report Size (1)
				0x05, 0x08,                         //     Usage Page (Page# for LEDs)
				0x19, 0x01,                         //     Usage Minimum (1)
				0x29, 0x05,                         //     Usage Maximum (5)
				0x91, 0x02,                         //     Output (Data, Variable, Absolute), Led report
				0x95, 0x01,                         //     Report Count (1)
				0x75, 0x03,                         //     Report Size (3)
				0x91, 0x01,                         //     Output (Data, Variable, Absolute), Led report padding

				0x95, 0x06,                         //     Report Count (6)
				0x75, 0x08,                         //     Report Size (8)
				0x15, 0x00,                         //     Logical Minimum (0)
				0x25, 0x65,                         //     Logical Maximum (101)
				
				0x05, 0x07,                         //     Usage Page (Key codes)
				0x19, 0x00,                         //     Usage Minimum (0)
				0x29, 0x65,                         //     Usage Maximum (101)
				0x81, 0x00,                         //     Input (Data, Array) Key array(6 bytes)


				0x09, 0x05,                         //     Usage (Vendor Defined)
				0x15, 0x00,                         //     Logical Minimum (0)
				0x26, 0xFF, 0x00,                   //     Logical Maximum (255)
				0x75, 0x08,                         //     Report Count (2)
				0x95, 0x02,                         //     Report Size (8 bit)
				0xB1, 0x02,                         //     Feature (Data, Variable, Absolute)

				0xC0                                // End Collection (Application)
};

struct pollfd pfds;

int uhid_write(int fd, const struct uhid_event *ev)
{
  ssize_t ret;

  ret = write(fd, ev, sizeof(*ev));
  if (ret < 0) {
    fprintf(stderr, "Cannot write to uhid: %m\n");
    return -errno;
  } else if (ret != sizeof(*ev)) {
    fprintf(stderr, "Wrong size written to uhid: %ld != %lu\n",
	    ret, sizeof(ev));
    return -EFAULT;
  } else {
    return 0;
  }
}

static int create(int fd)
{
  struct uhid_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.type = UHID_CREATE;
  strcpy((char*)ev.u.create.name, "test-uhid-device");
  ev.u.create.rd_data = rdesc;
  ev.u.create.rd_size = sizeof(rdesc);
  ev.u.create.bus = BUS_USB;
  ev.u.create.vendor = 0x15d9;
  ev.u.create.product = 0x0a37;
  ev.u.create.version = 0;
  ev.u.create.country = 0;
  printf("create success\n");
  return uhid_write(fd, &ev);
}

/* This parses raw output reports sent by the kernel to the device. A normal
 * uhid program shouldn't do this but instead just forward the raw report.
 * However, for ducomentational purposes, we try to detect LED events here and
 * print debug messages for it. */
void handle_output(struct uhid_event *ev)
{
  /* LED messages are adverised via OUTPUT reports; ignore the rest */
  if (ev->u.output.rtype != UHID_OUTPUT_REPORT)
    return;
  /* LED reports have length 2 bytes */
  if (ev->u.output.size != 2)
    return;
  /* first byte is report-id which is 0x02 for LEDs in our rdesc */
  if (ev->u.output.data[0] != 0x2)
    return;

  /* print flags payload */
  fprintf(stderr, "LED output report received with flags %x\n",
	  ev->u.output.data[1]);
}

int event(int fd)
{
  struct uhid_event ev;
  ssize_t ret;

  memset(&ev, 0, sizeof(ev));
  ret = read(fd, &ev, sizeof(ev));
  if (ret == 0) {
    fprintf(stderr, "Read HUP on uhid-cdev\n");
    return -EFAULT;
  } else if (ret < 0) {
    fprintf(stderr, "Cannot read uhid-cdev: %m\n");
    return -errno;
  } else if (ret != sizeof(ev)) {
    fprintf(stderr, "Invalid size read from uhid-dev: %ld != %lu\n",
	    ret, sizeof(ev));
    return -EFAULT;
  }

  switch (ev.type) {
  case UHID_START:
    fprintf(stderr, "UHID_START from uhid-dev\n");
    break;
  case UHID_STOP:
    fprintf(stderr, "UHID_STOP from uhid-dev\n");
    break;
  case UHID_OPEN:
    fprintf(stderr, "UHID_OPEN from uhid-dev\n");
    break;
  case UHID_CLOSE:
    fprintf(stderr, "UHID_CLOSE from uhid-dev\n");
    break;
  case UHID_OUTPUT:
    fprintf(stderr, "UHID_OUTPUT from uhid-dev\n");
    handle_output(&ev);
    break;
  case UHID_OUTPUT_EV:
    fprintf(stderr, "UHID_OUTPUT_EV from uhid-dev\n");
    break;
  default:
    fprintf(stderr, "Invalid event from uhid-dev: %u\n", ev.type);
  }

  return 0;
}

static int uhid_key_press(int fd, char key)
{
  struct uhid_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.type = UHID_INPUT;
  ev.u.input.size = 8;

  ev.u.input.data[2] = key;
  return uhid_write(fd, &ev);
}

int uhid_key_release(int fd)
{
  struct uhid_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.type = UHID_INPUT;
  ev.u.input.size = 8;
  return uhid_write(fd, &ev);
}

int uhid_key_event(int fd, char key, unsigned int holdtime)
{
  int ret = 1;

  ret = uhid_key_press(fd, key);
  if (ret < 0)
    return ret;
  usleep(holdtime * 1000);
  ret = uhid_key_release(fd);
  if (ret < 0)
    return ret;
  printf("key event %d\n", key);
  return 1;
}

int uhid_init(void)
{
  int ret;
  const char *path = "/dev/uhid";
  int fd;
  fd = open(path, O_RDWR | O_CLOEXEC);
  if (fd < 0) {
    fprintf(stderr, "Cannot open uhid-cdev %s: %m\n", path);
    return EXIT_FAILURE;
  }
  ret = create(fd);
  if (ret < 0) {
    close(fd);
    return EXIT_FAILURE;
  }


  pfds.fd = fd;
  pfds.events = POLLIN;
  return fd;
}


static void uhid_stop(int fd)
{
  struct uhid_event ev;

  memset(&ev, 0, sizeof(ev));
  ev.type = UHID_DESTROY;

  uhid_write(fd, &ev);
}

#ifdef _CFG_DEBUG_UHID_
int main(int argc, char **argv)
{
  int ret = 0;
  int cnt = 0;
  ret = uhid_init();
  if (ret < 0)
    return ret;
  
  while (true) {
    
    sleep(3);
    ret = uhid_key_event(pfds.fd, USB_KEY_W, 500);
    /*    
    if (cnt < 5) 
      ret = simkey(pfds.fd, KEY_A);
    else if (cnt < 8)
      ret = simkey(pfds.fd, KEY_D);
    else if (cnt < 11)
      ret = simkey(pfds.fd, KEY_S);
    cnt ++;
    if (cnt == 11)
      cnt = 0;
    if (ret < 0)
      return ret;
    */
  }
  uhid_stop(pfds.fd);
  return EXIT_SUCCESS;
}
#endif

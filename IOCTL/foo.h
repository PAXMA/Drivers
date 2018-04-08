#ifndef FOO_H
#define FOO_H

#include <linux/ioctl.h>

#define MAJOR_NUM 300
//#define IOCTL_CLEAR _IO(MAJOR_NUM, 0)
//#define IOCTL_CHANGE_BUF_LEN _IOR(MAJOR_NUM, 1, char *)
#define DEVICE_FILE_NAME "foo"

#endif


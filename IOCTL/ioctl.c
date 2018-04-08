#include "foo.h"    

#include <stdio.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */



/* Functions for the ioctl calls */

int IOCTL_CHANGE_BUF_LEN(int fl, char *message){
	int ret_val;
	ret_val = ioctl(fl, 1, message);
	if (ret_val < 0) {
		printf ("IOCTL_CHANGE_BUF_LEN failed:%d\n", ret_val);
		return -1;
  	}
  	return ret_val;
}

int IOCTL_CLEAR(int fl){
	int ret_val;
	ret_val = ioctl(fl, 0);
	if (ret_val < 0) {
		printf ("IOCTL_CLEAR failed:%d\n", ret_val);
		return -1;
	}
	return ret_val;
}

/* Main - Call the ioctl functions */
int main(){
	int fl, ret_val;
	char *msg = "100\n";
	fl = open(DEVICE_FILE_NAME, 0);
	if (fl < 0) {
		printf ("Can't open device file: %s\n", DEVICE_FILE_NAME);
    	return -1;
	}
	IOCTL_CLEAR(fl);
	IOCTL_CHANGE_BUF_LEN(fl, msg);
	close(fl);
	return 0;
}

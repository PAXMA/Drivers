#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define DEVICE_NAME "foo"
#define BUF_LEN 80
static int Major;
static char msg[BUF_LEN];
static char *msg_Ptr;
static char Device_Open = 0;

struct file_operations fops = {
	read: device_read,
	write: device_write,
 	open: device_open,
	release: device_release,
};

int init_module(){
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0){
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}
	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'sudo mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	return 0;
}

void cleanup_module(){
	unregister_chrdev(Major, DEVICE_NAME);
	printk(KERN_ALERT "Сleanup_module OK \n");
}

// cat /dev/foo
static int device_open(struct inode *inode, struct file *file){
	static int counter = 1;
	if (Device_Open)
		return -EBUSY;
	Device_Open++;
	if (counter == 1)
		sprintf(msg, "It is the %dst and last time this message is displayed\n", counter++);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode *inode, struct file *file){
	Device_Open--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *fl, char *buffer, size_t length, loff_t * offset){
	printk(KERN_INFO"Trying to read, given length is %d\n", length);
	/*if(length > 20){
		length = 20;
		printk(KERN_INFO"length forced to 20");
	}*/
	int bytes_read = 0;
	if (*msg_Ptr == 0)
		return 0;
	while (length && *msg_Ptr){
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}
	printk(KERN_INFO"READ\nbuffer: %s\nmsg: %s", buffer, msg);
	return bytes_read;
}

static ssize_t device_write(struct file *fl, const char *buffer, size_t length, loff_t * offset){
	printk(KERN_INFO"Trying to write %.*swith length %d\n",length, buffer, length);
	sprintf(msg, "%.*s", length, buffer);
	printk(KERN_INFO"WRITE\nbuffer: %s\nmsg: %s", buffer, msg);
	return length;
}



/*
unsigned long copy_to_user (void __user *to, const void *from, unsigned long n);
unsigned long copy_from_user (void *to, const void __user *from, unsigned long n);
*/

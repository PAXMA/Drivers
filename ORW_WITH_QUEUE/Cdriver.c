#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> /* Определения макросов */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h> /* put_user */
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PAXMA");

#define DEVICE_NAME "Cdriver"
#define CLASS_NAME "my_class"
#define SUCCESS 0
#define IOCTL_GET_SIZE_BUF 500
#define IOCTL_RESET_BUF 501
#define IOC_MAGIC 'h'

static int paramBB = 3;
module_param(paramBB, int, 0644);

//static int module_queue = 0;
static struct task_struct *wait_thread;
wait_queue_head_t readWait;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);
int wait_queue_flag = 0;

static int Major;
static int Device_Open = 0;
static int size_buf = 80;
static char msg[80];
static char* msg_Ptr = msg;
//static char length;
static int count = 0;

static int device_open( struct inode *, struct file * );
static int device_release( struct inode *, struct file * );
static ssize_t device_read( struct file *, char *, size_t, loff_t * );
static ssize_t device_write( struct file *, const char *, size_t, loff_t * );

//static int device_ioctl(struct inode *n, struct file *filp, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	// .ioctl = device_ioctl,
	.release = device_release
};

static struct device * dev;
static struct class * myClass;


int init_module(void){
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if(Major < 0){
    	printk(KERN_ALERT"Registering char device failed with %d\n", Major);
    }
    
    myClass = class_create(THIS_MODULE, CLASS_NAME);
    dev = device_create(myClass, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	//printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "param=%d\n", paramBB);
	// wait_thread = kthread_create((int)device_write, NULL, "WaitThread");
    return SUCCESS;
}

void cleanup_module(void){
	wait_queue_flag = 2;
	// kthread_stop(wait_thread);
	device_destroy(myClass, MKDEV(Major, 0));
	class_unregister(myClass);
	class_destroy(myClass);
	unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_INFO"Cleanup_module OK\n");
}

static int device_open(struct inode *inode, struct file *file){
	static int counter = 0;
	//if (Device_Open)
	//	return -EBUSY;
	Device_Open++;
	if(counter==0){
		sprintf(msg, "I already told you %d times Hello world!\n", counter++);
		msg_Ptr = msg;
	}
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file){
	Device_Open--; /* We're now ready for our next caller */
		/** Decrement the usage count, or else once you opened the file, you'll
			never get get rid of the module. */
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *file,char *buffer, size_t length, loff_t * offset){
    int to_read = count < length ? count : length;
    if(to_read == 0){
    	wait_event_killable(wait_queue, wait_queue_flag != 0);
    	// int to_write = length < size_buf ? length : size_buf;
    	// copy_from_user(msg, buffer, to_write);
    }
    to_read = count < length ? count : length;
    copy_to_user(buffer, msg, to_read);
    count -= to_read;
    return to_read;
}

// sudo bash -c 'echo hello > /dev/Cdriver'
static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t * offset){
	// while(buffer == 0)
 //    	printk(KERN_INFO "Waiting For Event...\n");
    int to_write = length < size_buf ? length : size_buf;
    copy_from_user(msg, buffer, to_write);
    count = count + to_write;
    wait_queue_flag = 1;
	wake_up(&wait_queue);
    return to_write;
}


// static int wait_function(const char *buffer, size_t length){
// 	printk(KERN_INFO "Waiting For Event...\n");
// 	//если пользователь хочет записать, должны разбудить устройство, но как понять, что пользователь хочет записать?
//     while(1){
//     	copy_from_user(msg, buffer, 1);
//     	if(buffer != 0){
//         	wait_queue_flag = 1;
//     		wake_up(&wait_queue);
//     	}
//         //wait_event_killable(wait_queue, wait_queue_flag != 0);
//     }
//     return 0;
// }
// static int device_ioctl(struct inode *n, struct file *filp, unsigned int cmd, unsigned long arg){
// 	if( ( _IOC_TYPE( cmd ) != IOC_MAGIC ) ) return -ENOTTY;
// 	switch( cmd ) {
//     	case IOCTL_GET_SIZE_BUF:
//     		return size_buf;

//     	case IOCTL_RESET_BUF:
//     		int i = 0;
//     		for (i = 0;i < size_buf; i++){
//     			msg[i] = '\0';
//     		}
// 			return 0;
//     	default: 
//         	return -ENOTTY;
//    }
//    return 0;
// }
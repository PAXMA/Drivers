#include <linux/module.h>
#include <linux/kernel.h>

int init_module(){
	printk(KERN_INFO "Hello, driver\n");
	return 0;
}

void cleanup_module(){
	printk(KERN_INFO "Bye, driver\n");
}

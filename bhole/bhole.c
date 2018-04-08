/*
После старта драйвера задать ему IP: sudo ifconfig bhnet0 10.1.1.1




*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

MODULE_LICENSE("GPL");

struct net_device *bhole_ndev;
struct bhole_priv{
	char tbuf[50];
};

static int bhole_send(struct sk_buff *skb, struct net_device *dev){
	static int i=0;
	printk(KERN_WARNING"bhole_send %d\n", i);
	printk(KERN_INFO"%s", *skb);
	printk(KERN_WARNING"bhole_send end of skb %d\n", i++);
	dev_kfree_skb(skb);
	return 0;
}

static int bhole_open(struct net_device* dev){
	printk(KERN_INFO"bhole_open\n");
	//memcpy(dev->dev_addr, "\0BHOLE", ETH_ALEN);
	
	netif_start_queue(dev);
	return 0;
}

static int bhole_stop(struct net_device *dev){
	netif_stop_queue(dev);
	return 0;
}

static struct net_device_ops bhole_ops = {
	.ndo_open = &bhole_open,
	.ndo_stop = &bhole_stop,
	.ndo_start_xmit = &bhole_send,
};

void bhole_init(struct net_device* dev){
	struct bhole_priv *priv;
	
	printk(KERN_INFO"before ether_setup\n");
	ether_setup(dev);
	printk(KERN_INFO"after ether_setup\n");
	
	dev->netdev_ops = &bhole_ops;
	
	dev->flags |= IFF_NOARP;
}

void bhole_cleanup(void){
	if(bhole_ndev){
		unregister_netdev(bhole_ndev);
		free_netdev(bhole_ndev);
	}
}

int bhole_init_module(void){
	int res = -1;
	printk(KERN_ALERT"Black hole netdev init module \n");
	bhole_ndev = alloc_etherdev(32);	//alloc_netdev(sizeof(struct bhole_priv), "bhnet%d", bhole_init);
	if(bhole_ndev == NULL){
		printk(KERN_ALERT"alloc_netdev failed\n");
		return res;
	}
	
	sprintf(bhole_ndev->name, "bhnet%d", 0);
	bhole_init(bhole_ndev);

	if((res = register_netdev(bhole_ndev)))
		printk("bhole: error %d registering driver", res);
		
	printk(KERN_INFO "sudo mknod /dev/%s c %d 0\n", bhole_ndev, res);
	
	return res;
}

module_init(bhole_init_module);
module_exit(bhole_cleanup);



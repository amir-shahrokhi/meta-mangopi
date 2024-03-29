#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/interrupt.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arash Golgol <arash.golgol@gmail.com>");
MODULE_DESCRIPTION("Simple GPIO interrupt counter ");
MODULE_VERSION("1.0.0");

#define DEVICE_NAME "irq-cnt"
#define CLASS_NAME "irq-cnt-class"
#define GPIO_PE0 128
//This used for storing the IRQ number for the GPIO
unsigned int GPIO_irqNumber;
unsigned int irq_cnt;
static struct device *irq_device;
static struct class *irq_class;
static dev_t irq_dev;

// debugging only
static ssize_t irq_cnt_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%d", irq_cnt);
}
static DEVICE_ATTR(irq_cnt, S_IRUSR, irq_cnt_show, NULL);

//Interrupt handler for GPIO. 
//This will be called whenever there is a raising edge detected. 
static irqreturn_t gpio_irq_handler(int irq,void *dev_id) 
{
  irq_cnt++;
  return IRQ_HANDLED;
}

static int __init irq_driver_init(void) {

    int rval;
    // cleanup counter
    irq_cnt = 0;

    //Input GPIO configuratioin
    //Checking the GPIO is valid or not
    if(gpio_is_valid(GPIO_PE0) == false){
        pr_err("GPIO %d is not valid\n", GPIO_PE0);
        goto r_gpio_in;
    }
  
    //Requesting the GPIO
    if(gpio_request(GPIO_PE0,"GPIO_PE0") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_PE0);
        goto r_gpio_in;
    }
  
    //configure the GPIO as input
    gpio_direction_input(GPIO_PE0);
  
    //Get the IRQ number for our GPIO
    GPIO_irqNumber = gpio_to_irq(GPIO_PE0);
    pr_info("GPIO_irqNumber = %d\n", GPIO_irqNumber);
  
    if (request_irq(GPIO_irqNumber,             //IRQ number
                    (void *)gpio_irq_handler,   //IRQ handler
                    IRQF_TRIGGER_RISING,        //Handler will be called in raising edge
                    "irq-counter",              //used to identify the device name using this IRQ
                    NULL)) {                    //device id for shared IRQ
        pr_err("my_device: cannot register IRQ ");
        goto r_gpio_in;
    }
  

/* Alloc a device region */
    rval = alloc_chrdev_region(&irq_dev, 1, 1, DEVICE_NAME);
    if (rval != 0)          /* error */
       goto cdev_alloc_err;

/* class */
    irq_class = class_create(CLASS_NAME);
    if (IS_ERR(irq_class)) {
        printk(KERN_ERR DEVICE_NAME " cant create class %s\n", CLASS_NAME);
        goto class_err;
    }

/* device */
    irq_device = device_create(irq_class, NULL, irq_dev, NULL, DEVICE_NAME);
    if (IS_ERR(irq_device)) {
        printk(KERN_ERR DEVICE_NAME " cant create device %s\n", DEVICE_NAME);
        goto device_err;
     }

/* device attribute on sysfs */
    rval = device_create_file(irq_device, &dev_attr_irq_cnt);
    if (rval < 0) {
        printk(KERN_ERR DEVICE_NAME " cant create device attribute %s %s\n", 
        DEVICE_NAME, dev_attr_irq_cnt.attr.name);
    }

    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

device_err:
    class_unregister(irq_class);
    class_destroy(irq_class);
cdev_alloc_err:
class_err:
    free_irq(GPIO_irqNumber, NULL);
r_gpio_in:
    gpio_free(GPIO_PE0);
    return -1;
}

/*
** Module exit function
*/
static void __exit irq_driver_exit(void)
{
    unregister_chrdev_region(irq_dev, 1);
    device_destroy(irq_class, irq_dev);
    class_unregister(irq_class);
    class_destroy(irq_class);
    free_irq(GPIO_irqNumber,NULL);
    gpio_free(GPIO_PE0);
    pr_info("Device Driver Remove...Done!!\n");
}

module_init(irq_driver_init);
module_exit(irq_driver_exit);


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

#define GPIO_PE0 128
//This used for storing the IRQ number for the GPIO
unsigned int GPIO_irqNumber;

//Interrupt handler for GPIO. 
//This will be called whenever there is a raising edge detected. 
static irqreturn_t gpio_irq_handler(int irq,void *dev_id) 
{
  pr_info("Interrupt Occurred\n");
  return IRQ_HANDLED;
}

static int __init irq_driver_init(void) {
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
  
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

r_gpio_in:
    gpio_free(GPIO_PE0);
    return -1;
}

/*
** Module exit function
*/
static void __exit irq_driver_exit(void)
{
    free_irq(GPIO_irqNumber,NULL);
    gpio_free(GPIO_PE0);
    pr_info("Device Driver Remove...Done!!\n");
}

module_init(irq_driver_init);
module_exit(irq_driver_exit);


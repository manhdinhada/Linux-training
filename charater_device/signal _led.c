#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Manh Dinh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple driver to control an LED using GPIO subsystem");
#define MODULE_NAME "led_driver"
#define MODULE_CLASS "devttt"

#define LED_GPIO 48 // Replace with the GPIO number for your LED

static int led_state = 0;
static dev_t dev;
static struct cdev *my_cdev;
static struct class *led_class;
static struct device *led_device;

static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("%s: Device opened\n", MODULE_NAME);
    return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
    pr_info("%s: Device closed\n", MODULE_NAME);
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buf, size_t len, loff_t *off)
{
    char kbuf[10]; // Buffer to store the LED state as a string
    int ret;

    if (*off > 0) // Check if offset is beyond the first read
        return 0;

    // Set the kbuf to "on\n" or "off\n" based on the LED state
    if (led_state == 1)
        snprintf(kbuf, sizeof(kbuf), "on\n");
    else
        snprintf(kbuf, sizeof(kbuf), "off\n");

    // Copy the data to user space
    ret = copy_to_user(buf, kbuf, strlen(kbuf));
    if (ret)
    {
        pr_err("%s: Failed to copy data to user\n", MODULE_NAME);
        return -EFAULT;
    }

    *off += strlen(kbuf); // Update the file offset

    pr_info("%s: LED state read as %s", MODULE_NAME, kbuf);
    return strlen(kbuf);
}

static ssize_t dev_write(struct file *filep, const char __user *buf, size_t len, loff_t *off)
{
    char kbuf[10];

    // Clear kbuf and copy data from user
    memset(kbuf, 0, sizeof(kbuf));
    if (copy_from_user(kbuf, buf, len))
    {
        pr_err("%s: Failed to copy data from user\n", MODULE_NAME);
        return -EFAULT;
    }

    kbuf[strcspn(kbuf, "\n")] = '\0';

    // Compare input and control LED
    if (strcmp(kbuf, "on") == 0)
    {
        gpio_set_value(LED_GPIO, 1);
        led_state = 1;
        pr_info("%s: LED turned ON\n", MODULE_NAME);
    }
    else if (strcmp(kbuf, "off") == 0)
    {
        gpio_set_value(LED_GPIO, 0);
        led_state = 0;
        pr_info("%s: LED turned OFF\n", MODULE_NAME);
    }
    else
    {
        pr_warn("%s: Invalid value received: %s\n", MODULE_NAME, kbuf);
        return -EINVAL;
    }

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_close,
    .read = dev_read,
    .write = dev_write,
};

static int __init init_led(void)
{
    int ret;

    // Request the GPIO for LED
    ret = gpio_request(LED_GPIO, "LED_GPIO");
    if (ret)
    {
        pr_err("%s: Failed to request GPIO %d\n", MODULE_NAME, LED_GPIO);
        return ret;
    }

    // Set the GPIO direction to output
    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret)
    {
        pr_err("%s: Failed to set GPIO direction\n", MODULE_NAME);
        gpio_free(LED_GPIO);
        return ret;
    }

    // Allocate character device region
    ret = alloc_chrdev_region(&dev, 0, 1, MODULE_NAME);
    if (ret < 0)
    {
        pr_err("%s: Failed to allocate char device region\n", MODULE_NAME);
        gpio_free(LED_GPIO);
        return ret;
    }

    my_cdev = cdev_alloc();
    if (!my_cdev)
    {
        pr_err("%s: Failed to allocate cdev\n", MODULE_NAME);
        unregister_chrdev_region(dev, 1);
        gpio_free(LED_GPIO);
        return -ENOMEM;
    }

    cdev_init(my_cdev, &fops);
    my_cdev->owner = THIS_MODULE;

    ret = cdev_add(my_cdev, dev, 1);
    if (ret < 0)
    {
        pr_err("%s: Failed to add cdev\n", MODULE_NAME);
        kfree(my_cdev);
        unregister_chrdev_region(dev, 1);
        gpio_free(LED_GPIO);
        return ret;
    }

    led_class = class_create(THIS_MODULE, MODULE_NAME);
    if (IS_ERR(led_class))
    {
        pr_err("%s: Failed to create class\n", MODULE_NAME);
        cdev_del(my_cdev);
        unregister_chrdev_region(dev, 1);
        gpio_free(LED_GPIO);
        kfree(my_cdev);
        return PTR_ERR(led_class);
    }

    led_device = device_create(led_class, NULL, dev, NULL, MODULE_NAME);
    if (IS_ERR(led_device))
    {
        pr_err("%s: Failed to create device\n", MODULE_NAME);
        class_destroy(led_class);
        cdev_del(my_cdev);
        unregister_chrdev_region(dev, 1);
        gpio_free(LED_GPIO);
        kfree(my_cdev);
        return PTR_ERR(led_device);
    }

    pr_info("%s: Device created successfully\n", MODULE_NAME);
    return 0;
}

static void __exit exit_led(void)
{
    gpio_set_value(LED_GPIO, 0); // Turn off the LED before exiting
    gpio_free(LED_GPIO);
    device_destroy(led_class, dev);
    class_destroy(led_class);
    cdev_del(my_cdev);
    unregister_chrdev_region(dev, 1);
    kfree(my_cdev);
    pr_info("%s: Module unloaded\n", MODULE_NAME);
}

module_init(init_led);
module_exit(exit_led);

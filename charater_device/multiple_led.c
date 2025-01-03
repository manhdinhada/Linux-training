#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Manh Dinh");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver to control 3 LEDs using separate device files");

#define MODULE_NAME "hello"
#define LED_CLASS_NAME "led_driver_class"
#define NUM_LEDS 3

// Define GPIO pins for LEDs
static int led_gpio_pins[NUM_LEDS] = {16, 17, 18};

// Device structures for each LED
struct led_device
{
    struct cdev cdev;
    int gpio_pin;
    char state[4]; // "on" or "off"
};

static struct led_device led_devices[NUM_LEDS];
static struct class *led_class;
static dev_t dev;

// File operations
static int dev_open(struct inode *inode, struct file *filep)
{
    struct led_device *led = container_of(inode->i_cdev, struct led_device, cdev);
    filep->private_data = led; // Store LED device in private data
    pr_info("%s: Device for GPIO %d opened\n", MODULE_NAME, led->gpio_pin);
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buf, size_t len, loff_t *off)
{
    struct led_device *led = filep->private_data;
    size_t state_len = strlen(led->state);

    if (*off != 0)
    {
        return 0; // EOF
    }

    if (len < state_len + 1)
    {
        return -EINVAL; // Buffer too small
    }

    if (copy_to_user(buf, led->state, state_len + 1))
    {
        return -EFAULT;
    }

    *off += state_len + 1;
    return state_len + 1;
}

static ssize_t dev_write(struct file *filep, const char __user *buf, size_t len, loff_t *off)
{
    char kbuf[10];
    struct led_device *led = filep->private_data;

    if (len >= sizeof(kbuf))
    {
        len = sizeof(kbuf) - 1;
    }

    memset(kbuf, 0, sizeof(kbuf));
    if (copy_from_user(kbuf, buf, len))
    {
        pr_err("%s: Failed to copy from user\n", MODULE_NAME);
        return -EFAULT;
    }

    kbuf[strcspn(kbuf, "\n")] = '\0'; // Remove trailing newline

    pr_info("%s: Write command received: %s\n", MODULE_NAME, kbuf); // Debug log

    if (strcmp(kbuf, "on") == 0)
    {
        if (gpio_set_value(led->gpio_pin, 1))
        {
            pr_err("%s: Failed to turn on GPIO %d\n", MODULE_NAME, led->gpio_pin);
            return -EINVAL;
        }
        strcpy(led->state, "on");
        pr_info("%s: GPIO %d LED turned ON\n", MODULE_NAME, led->gpio_pin);
    }
    else if (strcmp(kbuf, "off") == 0)
    {
        if (gpio_set_value(led->gpio_pin, 0))
        {
            pr_err("%s: Failed to turn off GPIO %d\n", MODULE_NAME, led->gpio_pin);
            return -EINVAL;
        }
        strcpy(led->state, "off");
        pr_info("%s: GPIO %d LED turned OFF\n", MODULE_NAME, led->gpio_pin);
    }
    else
    {
        pr_warn("%s: Invalid command: %s\n", MODULE_NAME, kbuf);
        return -EINVAL;
    }

    return len;
}

static int dev_close(struct inode *inode, struct file *filep)
{
    pr_info("%s: Device closed\n", MODULE_NAME);
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_close,
};

static int __init init_led_driver(void)
{
    int ret, i;

    // Allocate device numbers
    ret = alloc_chrdev_region(&dev, 0, NUM_LEDS, "leds_controller");
    if (ret < 0)
    {
        pr_err("%s: Failed to allocate device numbers\n", MODULE_NAME);
        return ret;
    }

    led_class = class_create(LED_CLASS_NAME);
    if (IS_ERR(led_class))
    {
        unregister_chrdev_region(dev, NUM_LEDS);
        return PTR_ERR(led_class);
    }

    for (i = 0; i < NUM_LEDS; i++)
    {
        led_devices[i].gpio_pin = led_gpio_pins[i];
        strcpy(led_devices[i].state, "off");

        // Request GPIO and configure it
        ret = gpio_request(led_devices[i].gpio_pin, MODULE_NAME);
        if (ret)
        {
            pr_err("%s: Failed to request GPIO %d\n", MODULE_NAME, led_devices[i].gpio_pin);
            goto err_gpio;
        }
        gpio_direction_output(led_devices[i].gpio_pin, 0); // Set GPIO as output

        // Initialize cdev
        cdev_init(&led_devices[i].cdev, &fops);
        led_devices[i].cdev.owner = THIS_MODULE;

        ret = cdev_add(&led_devices[i].cdev, MKDEV(MAJOR(dev), MINOR(dev) + i), 1);
        if (ret)
        {
            pr_err("%s: Failed to add cdev for GPIO %d\n", MODULE_NAME, led_devices[i].gpio_pin);
            return -1;
        }

        // Create device file
        if (IS_ERR(device_create(led_class, NULL, MKDEV(MAJOR(dev), MINOR(dev) + i), NULL, "led%d", i + 1)))
        {
            pr_err("%s: Failed to create device file /dev/led%d\n", MODULE_NAME, i + 1);
            cdev_del(&led_devices[i].cdev);
            gpio_free(led_devices[i].gpio_pin);
            return 1;
        }

        pr_info("%s: Device /dev/led%d created for GPIO %d\n", MODULE_NAME, i + 1, led_devices[i].gpio_pin);
    }

    return 0;
}

static void __exit exit_led_driver(void)
{
    int i;

    for (i = 0; i < NUM_LEDS; i++)
    {
        gpio_set_value(led_devices[i].gpio_pin, 0); // Turn off LED
        gpio_free(led_devices[i].gpio_pin);
        cdev_del(&led_devices[i].cdev);
        device_destroy(led_class, MKDEV(MAJOR(dev), MINOR(dev) + i));
    }

    class_destroy(led_class);
    unregister_chrdev_region(dev, NUM_LEDS);
    pr_info("%s: Module unloaded\n", MODULE_NAME);
}

module_init(init_led_driver);
module_exit(exit_led_driver);

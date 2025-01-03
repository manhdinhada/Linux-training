#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#define DEVICE_NAME "my_device"
#define CLASS_NAME "my_device_class"

static struct class *my_device_class = NULL;
static dev_t devno;
int i = 0;

struct my_device_data
{
    struct cdev cdev;
    struct device *device;
    dev_t dev;
    int id;
    const char *permission;
    const char *device_name;
};

// File operations structure
static int my_open(struct inode *inode, struct file *filep);
static ssize_t my_device_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t my_device_write(struct file *file, const char __user *buf, size_t len, loff_t *offset);

static struct file_operations my_device_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .read = my_device_read,
    .write = my_device_write,
};

// Probe function
static int my_device_probe(struct platform_device *pdev)
{
    struct my_device_data *data;
    struct device *dev = &pdev->dev;
    int ret;

    pr_info("Hello, World! Kernel module loaded.\n");

    // Allocate memory for device-specific data
    data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    // Read properties from device tree
    ret = of_property_read_u32(dev->of_node, "id", &data->id);
    if (ret)
    {
        pr_err("Failed to read 'id' property\n");
        return -EINVAL;
    }
    pr_info("ID: %d\n", data->id);

    ret = of_property_read_string(dev->of_node, "permission", &data->permission);
    if (ret)
    {
        pr_err("Failed to read 'permission' property\n");
        return -EINVAL;
    }
    pr_info("Permission: %s\n", data->permission);

    // Allocate device number dynamically
    ret = alloc_chrdev_region(&devno, 0, 1, pdev->name);
    if (ret)
    {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    // Initialize cdev
    cdev_init(&data->cdev, &my_device_fops);
    data->cdev.owner = THIS_MODULE;

    data->dev = MKDEV(MAJOR(devno), MINOR(devno) + i++);

    // Add cdev to system
    ret = cdev_add(&data->cdev, data->dev, 1);
    if (ret)
    {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(data->dev, 1);
        return ret;
    }

    // Create device class if not already created
    if (!my_device_class)
    {
        my_device_class = class_create(THIS_MODULE, CLASS_NAME);
        if (IS_ERR(my_device_class))
        {
            pr_err("Failed to create device class\n");
            cdev_del(&data->cdev);
            unregister_chrdev_region(data->dev, 1);
            return -1;
        }
    }

    // Create device
    data->device = device_create(my_device_class, NULL, data->dev, data, pdev->name);
    if (IS_ERR(data->device))
    {
        pr_err("Failed to create device\n");
        class_destroy(my_device_class);
        cdev_del(&data->cdev);
        unregister_chrdev_region(devno, 1);
        return PTR_ERR(data->device);
    }

    // Set platform_data
    data->device->platform_data = data;

    dev_info(dev, "Device initialized successfully\n");
    return 0;
}

// Remove function
static int my_device_remove(struct platform_device *pdev)
{
    struct my_device_data *data = pdev->dev.platform_data;

    // Cleanup on remove
    device_destroy(my_device_class, data->dev);
    cdev_del(&data->cdev);
    unregister_chrdev_region(data->dev, 1);

    dev_info(&pdev->dev, "Device removed\n");
    return 0;
}

// Open function
static int my_open(struct inode *inode, struct file *filep)
{
    struct my_device_data *data = container_of(inode->i_cdev, struct my_device_data, cdev);
    if (!data)
    {
        pr_err("Failed to get device data from cdev\n");
        return -ENODEV; // Thiết bị không tồn tại
    }

    filep->private_data = data;
    pr_info("Opened device %s\n", data->device_name);
    return 0;
}

// Read function
static ssize_t my_device_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    struct my_device_data *data = file->private_data;
    char message[256];

    len = snprintf(message, sizeof(message), "ID: %d, Permission: %s\n", data->id, data->permission);
    if (*offset >= len)
        return 0;

    if (copy_to_user(buf, message, len))
        return -EFAULT;

    *offset += len;
    return len;
}

// Write function
static ssize_t my_device_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    char *kernel_buffer;

    kernel_buffer = kmalloc(len + 1, GFP_KERNEL);
    if (!kernel_buffer)
        return -ENOMEM;

    if (copy_from_user(kernel_buffer, buf, len))
    {
        kfree(kernel_buffer);
        return -EFAULT;
    }

    kernel_buffer[len] = '\0'; // Null-terminate the string
    pr_info("Received data: %s\n", kernel_buffer);
    kfree(kernel_buffer);

    return len;
}

// Device tree match table
static const struct of_device_id my_device_dt_ids[] = {
    {.compatible = "myvendor,device1"},
    {.compatible = "myvendor,device2"},
    {},
};
// MODULE_DEVICE_TABLE(of, my_device_dt_ids);

// Platform driver structure
static struct platform_driver my_device_driver = {
    .probe = my_device_probe,
    .remove = my_device_remove,
    .driver = {
        .name = "my_device_driver",
        .of_match_table = my_device_dt_ids,
    },
};

module_platform_driver(my_device_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example Platform Driver for Devices");

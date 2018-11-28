#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEVICE_NAME "parta"
#define SUCCESS     0

// Globals
static int major;
static int parta_count;
static struct device* device_ptr;
static struct class* class_ptr;

// Declarations
static int parta_open(struct inode* inodep, struct file* filep); 
static ssize_t parta_read(struct file* filep, char* data, size_t len, loff_t* offset); 
static ssize_t parta_write(struct file* filep, const char* data, size_t len, loff_t* offset); 
static int parta_release(struct inode* inodep, struct file* filep);


struct file_operations fops = {
    .open = parta_open,
    .read = parta_read, 
    .write = parta_write,
    .release = parta_release
};


// Functions
static int parta_open(struct inode* inodep, struct file* filep) {
    pr_info("parta open\n");
    parta_count = 0;
    return SUCCESS;
}

static ssize_t parta_read(struct file* filep, char* data, size_t len, loff_t* offset) {
    return len;
}

static ssize_t parta_write(struct file* filep, const char* data, size_t len, loff_t* offset) {
    parta_count += len;
    pr_info("Count: %d\n", parta_count);
    return len;
}

static int parta_release(struct inode* inodep, struct file* filep) {
    pr_info("parta close\n");
    return SUCCESS;
}

static int __init parta_init(void) {
    pr_info("Initializing parta\n");

    // Register character device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if(major < 0) {
        pr_alert("parta device registration failed with %d", major);
        return major;
    }

    class_ptr = class_create(THIS_MODULE, "parta class");
    device_ptr = device_create(class_ptr, NULL, MKDEV(major, 0), NULL, "parta");

    pr_info("parta major: %d\n", major);

    return SUCCESS;
}

static void __exit parta_exit(void) {
    device_destroy(class_ptr, MKDEV(major, 0));
    class_unregister(class_ptr);
    class_destroy(class_ptr);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Exiting parta\n");
}

module_init(parta_init);
module_exit(parta_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GE");
MODULE_DESCRIPTION("Driver that counts characters, ENSC 351 A6 part A");

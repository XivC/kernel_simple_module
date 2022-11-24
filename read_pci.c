
 #include <linux/init.h>
 #include <linux/module.h>
 #include <linux/proc_fs.h>
 #include <linux/pci.h>

 #define BUFFER_SIZE 1024


static int vendor = 1;
static int dev = 1;


static void *proc;

static int  	__init lab_driver_init(void);
static void 	__exit lab_driver_exit(void);


static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t
length,loff_t * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len,
loff_t * off);


static struct file_operations proc_operations = {
        .read = read_proc,
        .write = write_proc,
};




static ssize_t read_proc(struct file *filp,
            char __user *user,
            size_t count,
            loff_t *ppos) {

    char buf[BUFFER_SIZE];
    int len = 0;
    struct pci_dev* pd = pci_get_device(vendor, dev, NULL);

    if (pd == NULL){
        len += sprintf(buf,"AHTUNG! No pci device found for vendor %d and device %d\n",vendor, dev);

        if (copy_to_user(user, buf, len)) {
            return -EIO;
        }
        *ppos = len;
        return len;
      }

    len += sprintf(buf, "class = %x\n", pd->class);
    len += sprintf(buf + len, "revision = %x\n", pd->revision);
    len += sprintf(buf + len, "PCIe capabilities = %lx\n", pd->pcie_cap);

    if (copy_to_user(user, buf, len)) {
        return -EIO;
    }
    *ppos = len;
    return len;
}


// zapis'
static ssize_t write_proc(struct file *filp, const char __user *user, size_t
count, loff_t *ppos) {
    int argc, content_len;
    char buf[BUFFER_SIZE];


    if (*ppos > 0 || count > BUFFER_SIZE){
        return -EIO;
    }

    if( copy_from_user(buf, user, count) ) {
        return -EIO;
    }

    argc = sscanf(buf, "%d %d", &vendor, &dev);

    if (argc != 2){
        return -EINVAL;
    }



    content_len = strlen(buf);
    *ppos = content_len;
    return content_len;
}

static int __init lab_driver_init(void) {


    proc = proc_create("lab_read_pci_dev", 0, NULL, &proc_operations);

    return 0;
}

static void __exit lab_driver_exit(void)
{

    proc_remove(proc);

}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("lab read pci_dev");
MODULE_VERSION("1.0");

module_init(lab_driver_init);
module_exit(lab_driver_exit);

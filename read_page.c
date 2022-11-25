 #include <linux/module.h>
 #include <linux/proc_fs.h>
 #include <linux/pid.h>
 #include <linux/memblock.h>
 #include <linux/mutex.h>

 #define BUFFER_SIZE 1024

static struct mutex mtx;

static int pid = 1;


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




static struct page *find_page(struct mm_struct* mm, unsigned long vaddr) {


    pgd_t *pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        return NULL;
    }

    p4d_t *p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        return NULL;
    }

    pud_t *pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        return NULL;
    }

    pmd_t *pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        return NULL;
    }

    pte_t *pte = pte_offset_kernel(pmd, vaddr);
    if (!pte) {
        return NULL;
    }

    return pte_page(*pte);


}


static size_t write_page_struct(char __user *user,
                struct task_struct *task_struct_ref) {
    char buf[BUFFER_SIZE];
    size_t len = 0;

    struct page *page_struct;
    struct mm_struct *mm = task_struct_ref->mm;


    if (mm == NULL) {
        sprintf(buf, "Task_struct's mm is NULL\n");
        return 0;
    }
    struct vm_area_struct *vm_current = mm->mmap;
    unsigned long start = vm_current->vm_start;
    unsigned long end = vm_current->vm_end;
    for (;start <= end; start += PAGE_SIZE) {
        page_struct = find_page(mm, start);
        if (page_struct != NULL) {
            len += sprintf(buf + len, "flags = %ld\n", page_struct->flags);
            len += sprintf(buf + len, "addr_space = %p\n", page_struct->mapping);
            len += sprintf(buf + len, "virtual addr = %lx\n", start);
            break;
        }
    }
    if (copy_to_user(user, buf, len)) {
        return -EIO;
    }
    return len;

}



static ssize_t read_proc(struct file *filp,
            char __user *user,
            size_t count,
            loff_t *ppos) {
    mutex_lock(&mtx);
    char buf[BUFFER_SIZE];
    int len = 0;
    struct task_struct *ts = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (*ppos > 0 || count < BUFFER_SIZE){
        return 0;
    }

    if (ts == NULL){
        len += sprintf(buf,"AHTUNG! No pages found for pid %d\n",pid);

        if (copy_to_user(user, buf, len)){
            return -EIO;
        }
        *ppos = len;
        return len;
    }

    len = write_page_struct(user, ts);


    *ppos = len;
    mutex_unlock(&mtx);
    return len;
}


// zapis'
static ssize_t write_proc(struct file *filp, const char __user *user, size_t
count, loff_t *ppos) {
    mutex_lock(&mtx);

    int argc, content_len;
    char buf[BUFFER_SIZE];


    if (*ppos > 0 || count > BUFFER_SIZE){
      mutex_unlock(&mtx);
        return -EIO;
    }

    if( copy_from_user(buf, user, count) ) {
      mutex_unlock(&mtx);
        return -EIO;
    }

    argc = sscanf(buf, "%d", &pid);
    if (argc != 1){
      mutex_unlock(&mtx);
        return -EINVAL;
    }



    content_len = strlen(buf);
    *ppos = content_len;
    mutex_unlock(&mtx);
    return content_len;
}

static int __init lab_driver_init(void) {

    mutex_init(&mtx);
    proc_create("lab_read_page", 0, NULL, &proc_operations);

    return 0;
}

static void __exit lab_driver_exit(void)
{

    remove_proc_entry("lab_read_page", NULL);

}


MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("lab read page");
MODULE_VERSION("1.0");

module_init(lab_driver_init);
module_exit(lab_driver_exit);

/*
 * create a file in /proc
 * playing around with the proc filesystem
 * 
 * 
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/seq_file.h> /* single_open */
#include <linux/proc_fs.h>
#include <linux/uaccess.h> /* copy_from_user */

// uname -r 4.2.0-27-generic ubuntu

// echo "whatever" > /proc/test/hello
// insmod testSum.ko
// rmmod testSum

// create a directory name test
static struct proc_dir_entry *root_test_dir;

#define MAX_PROC_SIZE 100
static char proc_data[MAX_PROC_SIZE];

static int test_hello_proc_show(struct seq_file *m, void *v)
{
	//seq_printf(m,"this is a test...say my name, say my name <get it?> \n");
	seq_printf(m,"%s\n",proc_data);
}

// ref. to Documentation/filesystems/seq_file.txt and code fs/seq_file.c : 564
static int hello_proc_open(struct inode *inode_ptr, struct file *fp)
{
	return single_open(fp,test_hello_proc_show, PDE_DATA(inode_ptr));
}

/* TODO this function could be improved significantly */
int hello_proc_write(struct file *file,const char *buf,int count,void *data )
{
	if(count > MAX_PROC_SIZE)
   	count = MAX_PROC_SIZE;

	if(copy_from_user(proc_data, buf, count))
    	return -EFAULT;

	return count;
}

// file operations ../include/linux/fs.h : 1670
static const struct file_operations test_hello_proc_fops = {
   //.owner = THIS_MODULE, /* for proc files this is not needed */
	.open = hello_proc_open,
	.read = seq_read,
	.write = hello_proc_write,
	.llseek = seq_lseek,
	.release = single_release,
};


// create root test directory
void init_test_proc(void){

   struct proc_dir_entry *pde;

	/* create /proc/test */
	root_test_dir = proc_mkdir("test",NULL);

	if(!root_test_dir)
		goto err1;

   /* create /proc/test/hello with read and write permissions for all ref. flags /include/linux/stat.h */ 
   pde = proc_create("test/hello",S_IRUGO | S_IWUGO,NULL,&test_hello_proc_fops);
	if(!pde)
		goto err2;

// TODO: put in some error handling 
err1:
	printk(KERN_INFO "kibilogic: placeholder error handling 1 \n");
err2:
	printk(KERN_INFO "kibilogic:  placeholder error handling 2 \n");

}

int proc_init(void)
{
	printk(KERN_INFO "kibilogic: module is now installed \n");
   init_test_proc();

	return 0;
}

/* for proc files exit routines are not needed */
void proc_cleanup(void)
{
	printk(KERN_INFO "kibilogic: the LKM is gone bye bye \n");

   remove_proc_entry("test", NULL);
   remove_proc_entry("test/hello", NULL);
}

module_init(proc_init);
module_exit(proc_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("B. Townsend");	
MODULE_DESCRIPTION("creating a proc entry");	

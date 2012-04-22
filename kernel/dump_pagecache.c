/*
 * =====================================================================================
 *
 *       Filename:  dump_mapping.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/23/2011 03:19:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (holi), hoakee@gmail.com
 *        Company:  Novatek Microelectronics Corp.
 *
 * =====================================================================================
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/writeback.h>
#include <linux/pagemap.h>
#include <asm/uaccess.h>
#include <asm/errno.h>

static unsigned char filename[128];

static int find_inode_by_name(const char *path, struct inode **inode)
{
	int err;
	struct nameidata nd;

	err = path_lookup(path, LOOKUP_FOLLOW, &nd);
        if (err)
		return err;

	*inode = nd.path.dentry->d_inode;
	if (!(*inode)) {
		err = -ENOENT;
		goto out;
	}

out:
	path_put(&nd.path);
	return err;
}

ssize_t pagecaches_write(struct file *filp, const char __user *buff, size_t len,  loff_t *offs )
{
        if (copy_from_user(&(filename), buff, len)) {
            return -EFAULT;
        }

        if (filename[len-1] == '\n') {
            filename[len-1] = '\0';
        }
    
        return len;
}



static int pagecaches_proc_show(struct seq_file *m, void *v)
{
        int err = 0;
        unsigned long index = 0, end_index = 0;
        struct inode *inode = NULL;
 	struct page *page = NULL;

        err = find_inode_by_name(filename,&inode);
        if(err || !(inode)) {
            printk(KERN_ERR "can not find inode of file: %s, errcode = %d\n", filename, err);
            return err; 
        }

        seq_printf(m, " Index         Offset            Status          \n");
        seq_printf(m, " ================================================\n");

	end_index = inode->i_size >> PAGE_CACHE_SHIFT;

	while (index <= end_index) {
		page = find_get_page(inode->i_mapping, index);
		if(!page) {
		    seq_printf(m, "%4ld: 0x%08lx - 0x%08lx  NOT In Cache.\n", index, index << PAGE_CACHE_SHIFT, ((index+1)<<PAGE_CACHE_SHIFT)-1);
                }
                else {
	            seq_printf(m, "%4ld: 0x%08lx - 0x%08lx  Cached, PFN=%ld\n", index, index << PAGE_CACHE_SHIFT, ((index+1)<<PAGE_CACHE_SHIFT)-1, page_to_pfn(page));
		    page_cache_release(page);
                }
                index++;
	}
        
        seq_printf(m, " ===============================================\n");
        seq_printf(m, " filename: %s, size: %ld bytes\n", filename, inode->i_size);
        seq_printf(m, " cached pages: %d, cached percent: %ld\n", inode->i_mapping->nrpages, (inode->i_mapping->nrpages)*100/(end_index+1));
    
        return 0;
}


static int pagecaches_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, pagecaches_proc_show, NULL);
}


static const struct file_operations pagecaches_proc_fops = {
        .open= pagecaches_proc_open,
        .read= seq_read,
        .write = pagecaches_write,
        .llseek= seq_lseek,
        .release= single_release,
};


static struct proc_dir_entry *pagecaches_entry = NULL;

static int __init pagecaches_init(void)
{
        int ret = 0;

        pagecaches_entry = create_proc_entry("pagecaches", 0777, NULL);
        if (pagecaches_entry == NULL) {
            ret = -ENOMEM;
            printk(KERN_ERR "pagecaches: can not create proc entry.\n");
        }
        else {
            pagecaches_entry->proc_fops = &pagecaches_proc_fops;
            //pagecaches_entry->read_proc  = pagecaches_read;
            //pagecaches_entry->write_proc = pagecaches_write;
            //pagecaches_entry->owner      = THIS_MODULE;
            printk(KERN_INFO "pagecaches: module loaded.\n");  
        }
        
        return ret;
}

static void __exit pagecaches_exit(void)
{
    remove_proc_entry("pagecaches", NULL);
    printk(KERN_INFO "pagecaches: module unloaded.\n");  
}


module_init(pagecaches_init);
module_exit(pagecaches_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ravi teja");


#if 0


static ssize_t filename_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
        memset(filename, 0, sizeof(filename));
        sprintf(filename, "%s", "/bin/busybox");
        return 0;
}






static int __init pagecaches_proc_init(void)
{
        proc_create("pagecaches", 0, NULL, &pagecaches_proc_fops);
        return 0;
}

module_init(pagecaches_proc_init);

#endif

#if 0
int pagecaches_read(char *buff, char **start, off_t off, int count, int *eof, void *data )
{
        int len = 0, err = 0;
        unsigned long index = 0, end_index = 0;
        struct inode *inode = NULL;
 	struct page *page = NULL;

        len += sprintf(buff, "filename: %s\n",filename);
        err = find_inode_by_name(filename,&inode);
        if(err || !(inode)) {
            printk(KERN_ERR "can not find inode of file: %s, errcode = %d\n", filename, err);
            return err; 
        }

	end_index = inode->i_size >> PAGE_CACHE_SHIFT;

	while (index <= end_index) {
		page = find_get_page(inode->i_mapping, index);
		if(!page) {
		    //len += sprintf(buff, "0x%8lx: NOT In Cache.\n", index << PAGE_CACHE_SHIFT);
		    printk(KERN_ERR "0x%8lx: NOT In Cache.\n", index << PAGE_CACHE_SHIFT);
		    
                    index++;
                    continue;
                }
	        //len += sprintf(buff, "0x%8lx: Cached. PFN=%ld\n", index << PAGE_CACHE_SHIFT, page_to_pfn(page));
	        printk(KERN_ERR "0x%8lx: Cached. PFN=%ld\n", index << PAGE_CACHE_SHIFT, page_to_pfn(page));
		page_cache_release(page);
		index++;
	}
    
        return len;
}
#endif

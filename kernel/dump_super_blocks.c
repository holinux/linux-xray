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

static void dump_super_block(struct super_block *sb, void *arg)
{
        struct inode *inode = NULL;
        struct seq_file *m = (struct seq_file *)arg;

        seq_printf(m,"==================================\n"); 
        seq_printf(m,"Super Block Info:\n");
        seq_printf(m,"dev_t: major = %d, minor = %d\n",MAJOR(sb->s_dev),MINOR(sb->s_dev));
        seq_printf(m,"blocksize = %ld\n",sb->s_blocksize);
        seq_printf(m,"s_id = %s\n", sb->s_id);
        seq_printf(m,"fs_name:%s\n",sb->s_type->name); 
        

        //list_for_each_entry(inode, &sb->s_inodes, i_sb_list) {
        //  seq_printf(m, "inode num: %ld, size: %ld\n", inode->i_ino, inode->i_size); 
            //(inode->i_mapping->nrpages == 0)) {
        //}
}
  
#if 0
/**
 *      iterate_supers - call function for all active superblocks
 *      @f: function to call
 *      @arg: argument to pass to it
 *
 *      Scans the superblock list and calls given function, passing it
 *      locked superblock and given argument.
 */
void iterate_supers(void (*f)(struct super_block *, void *), void *arg)
{
    struct super_block *sb, *p = NULL;
    
    spin_lock(&sb_lock);
    list_for_each_entry(sb, &super_blocks, s_list) {
        if (list_empty(&sb->s_instances))
            continue;
        sb->s_count++;
        spin_unlock(&sb_lock);
        
        down_read(&sb->s_umount);
        if (sb->s_root)
            f(sb, arg);
        up_read(&sb->s_umount);
        
        spin_lock(&sb_lock);
        //if (p)
        //    __put_super(p);
        p = sb;
   }
  // if (p)
    //   __put_super(p);
   spin_unlock(&sb_lock);
}
#endif

static int super_blocks_show(struct seq_file *m, void *v)
{
    iterate_supers(dump_super_block, (void*)m);
    return 0;
}

static int super_blocks_open(struct inode *inode, struct file *file)
{
        return single_open(file, super_blocks_show, NULL);
}


static const struct file_operations super_blocks_fops = {
        .open= super_blocks_open,
        .read= seq_read,
        .llseek= seq_lseek,
        .release= single_release,
};

static struct proc_dir_entry *super_blocks_entry = NULL;

static int __init super_blocks_init(void)
{
        int ret = 0;

        super_blocks_entry = create_proc_entry("super_blocks", 0777, NULL);
        if (super_blocks_entry == NULL) {
            ret = -ENOMEM;
            printk(KERN_ERR "super_blocks: can not create proc entry.\n");
        }
        else {
            super_blocks_entry->proc_fops = &super_blocks_fops;
            printk(KERN_INFO "super_blocks: module loaded.\n");  
        }
        
        return ret;
}

static void __exit super_blocks_exit(void)
{
    remove_proc_entry("super_blocks", NULL);
    printk(KERN_INFO "super_blocks: module unloaded.\n");  
}


module_init(super_blocks_init);
module_exit(super_blocks_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ravi teja");

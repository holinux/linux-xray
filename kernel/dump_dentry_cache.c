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
#include <linux/dcache.h>
#include <asm/uaccess.h>
#include <asm/errno.h>


extern struct hlist_bl_head *dentry_hashtable __read_mostly;


static void dump_super_block(struct super_block *sb, void *arg)
{
        struct dentry *dentry = NULL;
        struct seq_file *m = (struct seq_file *)arg;
    
        seq_printf(m,"==================================\n"); 
        seq_printf(m,"Super Block Info:\n");
        seq_printf(m,"dev_t: major = %d, minor = %d\n",MAJOR(sb->s_dev),MINOR(sb->s_dev));
        seq_printf(m,"blocksize = %ld\n",sb->s_blocksize);
        seq_printf(m,"s_id = %s\n", sb->s_id);
        seq_printf(m,"fs_name:%s\n",sb->s_type->name); 
        
        list_for_each_entry(dentry, &sb->s_dentry_lru, d_lru)
        {
            seq_printf(m,"number: name: %s\n", dentry->d_name.name);
            seq_printf(m,"hash: %d\n", dentry->d_name.hash);
        }

}

static int dentry_cache_show(struct seq_file *m, void *v)
{
        struct hlist_node *node;
        struct dentry *dentry;
        int loop = 0;      

    iterate_supers(dump_super_block, (void*)m);
        
#if 0        
        
        //   rcu_read_lock();
        
//        seq_printf(m,"dhash_entries: %d\n", dhash_entries);
        
        //for (loop = 0; loop < (1 << d_hash_shift); loop++)
        for (loop = 0; loop < 65536; loop++)
        {
            if (hlist_empty(&dentry_hashtable[loop])) 
                continue;
            seq_printf(m,"===== entrie number: %d =====\n", loop);

            hlist_for_each_entry_rcu(dentry, node, &dentry_hashtable[loop], d_hash)
            {
		//struct qstr *qstr;

	        seq_printf(m,"number: name: %s\n", dentry->d_name.name);
                seq_printf(m,"hash: %d\n", dentry->d_name.hash);

                //if (dentry->d_name.hash != hash)
		//	continue;
		//if (dentry->d_parent != parent)
		//	continue;
#if 0
		spin_lock(&dentry->d_lock);

		/*
		 * Recheck the dentry after taking the lock - d_move may have
		 * changed things.  Don't bother checking the hash because we're
		 * about to compare the whole name anyway.
		 */
		if (dentry->d_parent != parent)
			goto next;

		/* non-existing due to RCU? */
		if (d_unhashed(dentry))
			goto next;

		/*
		 * It is safe to compare names since d_move() cannot
		 * change the qstr (protected by d_lock).
		 */
		qstr = &dentry->d_name;
		if (parent->d_op && parent->d_op->d_compare) {
			if (parent->d_op->d_compare(parent, qstr, name))
				goto next;
		} else {
			if (qstr->len != len)
				goto next;
			if (memcmp(qstr->name, str, len))
				goto next;
		}

		atomic_inc(&dentry->d_count);
		found = dentry;
		spin_unlock(&dentry->d_lock);
		break;
next:
		spin_unlock(&dentry->d_lock);
#endif
            }
        }
// 	rcu_read_unlock();
#endif
    return 0;
}

static int dentry_cache_open(struct inode *inode, struct file *file)
{
        return single_open(file, dentry_cache_show, NULL);
}


static const struct file_operations dentry_cache_fops = {
        .open= dentry_cache_open,
        .read= seq_read,
        .llseek= seq_lseek,
        .release= single_release,
};

static struct proc_dir_entry *dentry_cache_entry = NULL;

static int __init dentry_cache_init(void)
{
        int ret = 0;

        dentry_cache_entry = create_proc_entry("dentry_cache", 0777, NULL);
        if (dentry_cache_entry == NULL) {
            ret = -ENOMEM;
            printk(KERN_ERR "dentry_cache: can not create proc entry.\n");
        }
        else {
            dentry_cache_entry->proc_fops = &dentry_cache_fops;
            printk(KERN_INFO "dentry_cache: module loaded.\n");  
        }
        
        return ret;
}

static void __exit dentry_cache_exit(void)
{
    remove_proc_entry("dentry_cache", NULL);
    printk(KERN_INFO "dentry_cache: module unloaded.\n");  
}


module_init(dentry_cache_init);
module_exit(dentry_cache_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ravi teja");

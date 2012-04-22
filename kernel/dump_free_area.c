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

#define K(x) ((x) << (PAGE_SHIFT-10))

static char * const migratetype_names[MIGRATE_TYPES] = {
    "Unmovable",
    "Reclaimable",
    "Movable",
    "Reserve",
    "Isolate",
};


static int dump_free_area_proc_show(struct seq_file *m, void *v)
{
        struct zone *zone = NULL;
 	struct page *page = NULL;
       
        for_each_populated_zone(zone) {
 		unsigned long nr[MAX_ORDER], flags, order,type, pfn, total = 0;

		spin_lock_irqsave(&zone->lock, flags);
		for (order = 0; order < MAX_ORDER; order++) {
			nr[order] = zone->free_area[order].nr_free;
			total += nr[order] << order;

                        for (type = 0; type < MIGRATE_TYPES; type++)
                        {
                            seq_printf(m, "zone %8s,order %2lu,type %12s ", zone->name, order, migratetype_names[type]);

                            list_for_each_entry(page,&(zone->free_area[order].free_list[type]),lru) 
                            {
                                pfn = page_to_pfn(page);
                                seq_printf(m, "(%lu,%lu)<->", pfn, pfn+(1<<order)); 
                            }
                            seq_printf(m, "\n");
                        }
		}
		spin_unlock_irqrestore(&zone->lock, flags);
		for (order = 0; order < MAX_ORDER; order++)
		        seq_printf(m, "%lu*%lukB\n", nr[order], K(1UL) << order);
		seq_printf(m, "= %lukB\n", K(total));
	}

        return 0;
}


static int dump_free_area_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, dump_free_area_proc_show, NULL);
}


static const struct file_operations dump_free_area_proc_fops = {
        .open= dump_free_area_proc_open,
        .read= seq_read,
//        .write = dump_free_area_write,
        .llseek= seq_lseek,
        .release= single_release,
};


static struct proc_dir_entry *dump_free_area_entry = NULL;

static int __init dump_free_area_init(void)
{
        int ret = 0;

        dump_free_area_entry = create_proc_entry("dump_free_area", 0777, NULL);
        if (dump_free_area_entry == NULL) {
            ret = -ENOMEM;
            printk(KERN_ERR "dump_free_area: can not create proc entry.\n");
        }
        else {
            dump_free_area_entry->proc_fops = &dump_free_area_proc_fops;
            //dump_free_area_entry->read_proc  = dump_free_area_read;
            //dump_free_area_entry->write_proc = dump_free_area_write;
            //dump_free_area_entry->owner      = THIS_MODULE;
            printk(KERN_INFO "dump_free_area: module loaded.\n");  
        }
        
        return ret;
}

static void __exit dump_free_area_exit(void)
{
    remove_proc_entry("dump_free_area", NULL);
    printk(KERN_INFO "dump_free_area: module unloaded.\n");  
}


module_init(dump_free_area_init);
module_exit(dump_free_area_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ravi teja");


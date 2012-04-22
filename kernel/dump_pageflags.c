/*
 * =====================================================================================
 *
 *       Filename:  dump_pageflags.c
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  12/07/2011 05:49:39 PM
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

static struct trace_print_flags pageflag_names[] = {
	{1UL << PG_locked,		"locked"	},
	{1UL << PG_error,		"error"		},
	{1UL << PG_referenced,		"referenced"	},
	{1UL << PG_uptodate,		"uptodate"	},
	{1UL << PG_dirty,		"dirty"		},
	{1UL << PG_lru,			"lru"		},
	{1UL << PG_active,		"active"	},
	{1UL << PG_slab,		"slab"		},
	{1UL << PG_owner_priv_1,	"owner_priv_1"	},
	{1UL << PG_arch_1,		"arch_1"	},
	{1UL << PG_reserved,		"reserved"	},
	{1UL << PG_private,		"private"	},
	{1UL << PG_private_2,		"private_2"	},
	{1UL << PG_writeback,		"writeback"	},
#ifdef CONFIG_PAGEFLAGS_EXTENDED
	{1UL << PG_head,		"head"		},
	{1UL << PG_tail,		"tail"		},
#else
	{1UL << PG_compound,		"compound"	},
#endif
	{1UL << PG_swapcache,		"swapcache"	},
	{1UL << PG_mappedtodisk,	"mappedtodisk"	},
	{1UL << PG_reclaim,		"reclaim"	},
	{1UL << PG_swapbacked,		"swapbacked"	},
	{1UL << PG_unevictable,		"unevictable"	},
#ifdef CONFIG_MMU
	{1UL << PG_mlocked,		"mlocked"	},
#endif
#ifdef CONFIG_ARCH_USES_PG_UNCACHED
	{1UL << PG_uncached,		"uncached"	},
#endif
#ifdef CONFIG_MEMORY_FAILURE
	{1UL << PG_hwpoison,		"hwpoison"	},
#endif
	{-1UL,				NULL		},
};

static void dump_page_flags(struct seq_file* m, unsigned long flags)
{
	const char *delim = "";
	unsigned long mask;
	int i;

	seq_printf(m, "page flags: %#lx(", flags);

	/* remove zone id */
	flags &= (1UL << NR_PAGEFLAGS) - 1;

	for (i = 0; pageflag_names[i].name && flags; i++) {

		mask = pageflag_names[i].mask;
		if ((flags & mask) != mask)
			continue;

		flags &= ~mask;
		seq_printf(m, "%s%s", delim, pageflag_names[i].name);
		delim = "|";
	}

	/* check for left over flags */
	if (flags)
		seq_printf(m, "%s%#lx", delim, flags);

	seq_printf(m, ")\n");
}

static unsigned int start_pfn;

ssize_t pageflags_write(struct file *filp, const char __user *buff, size_t len,  loff_t *offs )
{
        unsigned char str[30];
        
        if (copy_from_user(str, buff, len)) {
            return -EFAULT;
        }

        if (str[len-1] == '\n') {
            str[len-1] = '\0';
        }

        start_pfn = simple_strtoul(str, NULL, 10);

        return len;
}


static int pageflags_proc_show(struct seq_file *m, void *v)
{
        struct page * page;
        page = pfn_to_page(start_pfn);

        seq_printf(m, "pfn:%lu page:%p count:%d mapcount:%d mapping:%p index:%#lx\n",
		start_pfn, page, atomic_read(&page->_count), page_mapcount(page),
		page->mapping, page->index);
	dump_page_flags(m, page->flags);
	//mem_cgroup_print_bad_page(page);
        
        return 0;
}

static int pageflags_proc_open(struct inode *inode, struct file *file)
{
        return single_open(file, pageflags_proc_show, NULL);
}


static const struct file_operations pageflags_proc_fops = {
        .open    = pageflags_proc_open,
        .read    = seq_read,
        .write   = pageflags_write,
        .llseek  = seq_lseek,
        .release = single_release,
};


static struct proc_dir_entry *pageflags_entry = NULL;

static int __init pageflags_init(void)
{
        int ret = 0;

        pageflags_entry = create_proc_entry("pageflags", 0777, NULL);
        if (pageflags_entry == NULL) {
            ret = -ENOMEM;
            printk(KERN_ERR "pageflags: can not create proc entry.\n");
        }
        else {
            pageflags_entry->proc_fops = &pageflags_proc_fops;
            //pageflags_entry->read_proc  = pageflags_read;
            //pageflags_entry->write_proc = pageflags_write;
            //pageflags_entry->owner      = THIS_MODULE;
            printk(KERN_INFO "pageflags: module loaded.\n");  
        }
        
        return ret;
}

static void __exit pageflags_exit(void)
{
    remove_proc_entry("pageflags", NULL);
    printk(KERN_INFO "pageflags: module unloaded.\n");  
}


module_init(pageflags_init);
module_exit(pageflags_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ravi teja");


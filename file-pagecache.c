#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shiqi");
MODULE_DESCRIPTION("file pagecache");

extern void iterate_supers(void (*)(struct super_block *, void *), void *);

static void sb_type(struct super_block *sb, void *arg)
{
    if (sb->s_type)
        printk("name %s \n", sb->s_type->name);
}

static int __init file_pagecache_init(void)
{
    printk("hello world!\n");
    iterate_supers(sb_type, NULL);
    return 0;
}

static void __exit file_pagecache_exit(void)
{
    printk("hello exit!\n");
}

module_init(file_pagecache_init);
module_exit(file_pagecache_exit);

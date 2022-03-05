#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>
#include "mount.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("qiancan");
MODULE_DESCRIPTION("file pagecache");

struct file_system_type *gptype;
static const char* filesystem[] = {
"ext4", "cramfs", "ubifs", "yaffs2", "yaffs",
"proc", "sysfs", "devtmpfs", "devpts", "debugfs",
"tmpfs", "squashfs"
};
static char* filesystem_name;
module_param(filesystem_name, charp, 0);

static struct vfsmount *get_dentry_mnt(struct dentry *dentry)
{
    struct vfsmount *p, *root;
    struct fs_struct *fs = current->fs;
    struct mount *mnt;

    spin_lock(&fs->lock);
    root = fs->root.mnt;
    mntget(root);
    spin_unlock(&fs->lock);

    if (root->mnt_sb == dentry->d_sb)
    {
        return root;
    }

    list_for_each_entry(mnt, &real_mount(root)->mnt_list, mnt_list)
    {
        p = &mnt->mnt;
        if (p->mnt_sb == dentry->d_sb)
        {
            mntget(p);
            break;
        }
    }

    mntput(root);
    return p;
}

static char* get_dentry_path(struct dentry *dentry, char *buf, int len)
{
    char *p = NULL;
    struct vfsmount *mnt = get_dentry_mnt(dentry);
    struct path ph;

    if (mnt)
    {
        ph.dentry = dentry;
        ph.mnt = mnt;
        p = d_path(&ph, buf, len);
        if (IS_ERR(p))
        {
            p = NULL;
        }
        mntput(mnt);
    }

    return p;
}

static void sb_type(struct super_block *sb, void *arg)
{
    struct inode *inode = NULL;
    struct dentry *dentry = NULL;
    char name[256];
    int nrpages = 0;
    char* pret = NULL;

    spin_lock(&sb->s_inode_list_lock);
    list_for_each_entry(inode, &sb->s_inodes, i_sb_list)
    {
        nrpages = inode->i_mapping->nrpages;
        dentry = d_find_alias(inode);
        if (dentry)
        {
            spin_lock(&dentry->d_lock);
            pret = get_dentry_path(dentry, name, 256);
            spin_unlock(&dentry->d_lock);
            dput(dentry);
            if (pret)
            {
                printk("[file-pagecache]%s:pagecache: %d\n", pret, nrpages);
            }
        }
    }
    spin_unlock(&sb->s_inode_list_lock);
}

static int __init file_pagecache_init(void)
{
    int i = 0;
    int count = 0;

    printk("hello world!\n");
    
    if (filesystem_name)
    {
        printk("filesystem name : %s \n", filesystem_name);
        gptype = get_fs_type(filesystem_name);
        if (gptype)
        {
            iterate_supers_type(gptype, sb_type, NULL);
        }
        return 0;
    }
    
    count = ARRAY_SIZE(filesystem);
    for (i = 0; i < count ; i++)
    {
        printk("-------------------------------%s------------------------------\n", filesystem[i]);
        gptype = get_fs_type(filesystem[i]);
        if (gptype)
        {
            iterate_supers_type(gptype, sb_type, NULL);
        }
    }
    return 0;
}

static void __exit file_pagecache_exit(void)
{
    printk("hello exit!\n");
}

module_init(file_pagecache_init);
module_exit(file_pagecache_exit);

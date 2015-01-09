/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * include/filesystem.h
 *
 * FAT32 Filesystem
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	December 13 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define FS_MAX_NAME 255

#include <types.h>

/*
 * FAT32 volume
 */
struct vol_t {
	unsigned vol_lba;	/* LBA of volume */
	unsigned size;		/* size of volume in sectors */
	unsigned sector_size;	/* size of sector in bytes */
	unsigned cluster_size;	/* size of cluster in sectors */
	unsigned fat_size;	/* size of FAT in sectors */
	unsigned num_fats;	/* number of FATs */
	unsigned cluster_lba;	/* LBA of first cluster */
	unsigned fat_lba;	/* LBA of first FAT */
	unsigned root;	/* first cluster of root directory */
};

/*
 * FAT32 directory entry
 */
struct dirent_t {
	char short_name[12];		/* FAT32 short name */
	char long_name[FS_MAX_NAME+1];	/* FAT32 long name */
	unsigned cluster;		/* first cluster */
	unsigned size;
};

/* Function Prototypes */
void fs_init();
void fs_dump_part_table();
int fs_read(const char *filename, unsigned char* buf, size_t off, size_t count);

#endif /* FILESYSTEM_H */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 *
 * src/filesystem.c
 *
 * FAT32 filesystem
 *
 * Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
 * Date:	December 13 2014
 *
 * Copyright (c) 2014, Daniel Kudrow
 * All rights reserved, see LICENSE.txt for details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 */

#include <emmc.h>
#include <util.h>
#include <filesystem.h>
#include <types.h>

#ifdef DEBUG_FS
#define PRINT_DEBUG
#endif
#define PRINT_HANDLE "FS"
#include "debug.h"

/*
 * FAT32 volume
 */
struct fat32_vol_t {
	unsigned vol_lba;	/* LBA of volume */
	unsigned size;		/* size of volume in sectors */
	size_t sector_size;	/* size of sector in bytes */
	unsigned cluster_size;	/* size of cluster in sectors */
	unsigned fat_size;	/* size of FAT in sectors */
	unsigned num_fats;	/* number of FATs */
	unsigned cluster_lba;	/* LBA of first cluster */
	unsigned fat_lba;	/* LBA of first FAT */
	unsigned root;		/* first cluster of root directory */
};

/*
 * FAT32 long and short names
 */
struct fat32_dirent_t {
	char short_name[13];		/* FAT32 short name */
	char long_name[FS_MAX_NAME+1];	/* FAT32 long name */
	unsigned cluster;		/* first cluster */
};

/*
 * On disk layout of MS DOS partition table entry
 */
struct msdos_part_t {
	uint8_t boot;
	uint8_t _PAD1[7];
	uint32_t start_lba;
	uint32_t size;
} __attribute__((packed));

/*
 * On disk layout of MBR
 */
struct mbr_t {
	uint8_t _PAD1[446];
	struct msdos_part_t part_1;
	struct msdos_part_t part_2;
	struct msdos_part_t part_3;
	struct msdos_part_t part_4;
	uint16_t magic;
} __attribute__((packed));

/*
 * On disk layout of Volume ID/Boot Parameter Block sector
 */
struct fat32_bpb_t {
	uint8_t _PAD1[11];
	uint16_t sector_size;	/* size of sector in bytes */
	uint8_t cluster_size;	/* size of cluster in sectors */
	uint16_t reserved_sectors;/* resrved sectors before FAT */
	uint8_t num_fats;		/* replication of FAT */
	uint8_t _PAD2[19];
	uint32_t fat_size;		/* size of FAT in sectors */
	uint8_t _PAD3[4];
	uint32_t root;			/* cluster containing root directory */
} __attribute__((packed));

/*
 * On disk layout of long directory entry
 */
struct fat32_long_dirent_t {
	uint8_t sequence;		/* sequence of entry in long name */
	uint16_t name_1[5];	/* characters 1-5 in entry */
	uint8_t attributes;	/* should always be 0x0F */
	uint8_t type;		/* should always be 0x00 */
	uint8_t checksum;		/* checksum of short name -- ignore */
	uint16_t name_2[6];	/* characters 6-11 in entry */
	uint8_t _PAD[2];		/* should always be 0x0000 */
	uint16_t name_3[2];	/* characters 12-13 in entry */
} __attribute__((packed));

#define FAT32_IS_LONG(dirent) (READ_1(((uint8_t *)dirent)+11) == 0x0F)

/*
 * On disk layout of short directory entry
 */
struct fat32_short_dirent_t {
	char name[11];			/* short name in ABCDEFGH.EXT format */
	uint8_t attributes;	/* file attributes */
	uint8_t _PAD;		/* should always be 0x00 */
	uint8_t time_tenths;	/* 0.1s creation time */
	uint16_t creation_time;	/* creation time */
	uint16_t creation_date;	/* creation date */
	uint16_t access_date;	/* last access date */
	uint16_t cluster_hi;	/* hi bytes of first cluster number */
	uint16_t write_time;	/* last write time */
	uint16_t write_date;	/* last write date */
	uint16_t cluster_lo;	/* lo bytes of first cluster number */
	uint32_t size;			/* size of file in bytes */
} __attribute__((packed));

struct fat32_vol_t volume;

/*
 * Load a cluster into a buffer
 */
void fat32_get_cluster(unsigned logical_cluster, unsigned char *buf)
{
	int i;
	unsigned absolute_cluster = logical_cluster - volume.root;
	unsigned lba = volume.cluster_lba + absolute_cluster * volume.cluster_size;
	for (i=0; i<volume.cluster_size; i++)
		emmc_read_block(lba+i, buf+i*volume.sector_size);
}

/*
 * Read an MS DOS format partition table
 */
void fat32_init()
{
	unsigned char sector[512];
	struct mbr_t *mbr = (struct mbr_t *)sector;
	struct msdos_part_t *part = &mbr->part_1;
	struct fat32_bpb_t *bpb = (struct fat32_bpb_t *)sector;

	/* find volume in partition table */
	emmc_read_block(0, sector);
	volume.vol_lba = part->start_lba;
	volume.size = part->size;

	/* initialize volume from BIOS Paramter Block */
	emmc_read_block(volume.vol_lba, sector);
	volume.sector_size = bpb->sector_size;
	volume.cluster_size = bpb->cluster_size;
	volume.fat_size = bpb->fat_size;
	volume.num_fats = bpb->num_fats;
	volume.fat_lba = volume.vol_lba + bpb->reserved_sectors;
	volume.cluster_lba = volume.fat_lba + volume.num_fats * volume.fat_size;
	volume.root = bpb->root;
}

/*
 * Read the directory entry in cluster at offset
 */
unsigned fat32_readdir(unsigned char *cluster, unsigned offset,
		struct fat32_dirent_t *dirent)
{
	int i, pos, seq;
	struct fat32_short_dirent_t *short_dirent;
	struct fat32_long_dirent_t *long_dirent =
		(struct fat32_long_dirent_t *)(cluster + offset);

	dirent->long_name[0] = '\0';
	dirent->short_name[0] = '\0';

	/* check for empty directory entry */
	if ((cluster + offset)[0] == 0xE5)
		return offset + sizeof(struct fat32_short_dirent_t);

	/* check for last directory entry */
	if ((cluster + offset)[0] == 0x00)
		return 0;

	/* build the long name */
	seq = ((long_dirent->sequence & ~0x40) - 1) * 13;
	dirent->long_name[seq+13] = '\0';
	while (FAT32_IS_LONG(long_dirent)) {
		pos = 0;
		for (i=0; i<5; i++)
			dirent->long_name[seq+pos++] = (char)long_dirent->name_1[i];
		for (i=0; i<6; i++)
			dirent->long_name[seq+pos++] = (char)long_dirent->name_2[i];
		for (i=0; i<2; i++)
			dirent->long_name[seq+pos++] = (char)long_dirent->name_3[i];
		long_dirent++;
		seq = (long_dirent->sequence - 1) * 13;
	}

	/* build the short name */
	short_dirent = (struct fat32_short_dirent_t *)long_dirent;
	strncpy(&dirent->short_name[0], short_dirent->name, 8);
	dirent->short_name[8] = '.';
	strncpy(&dirent->short_name[9], &short_dirent->name[8], 3);
	dirent->short_name[12] = '\0';

	dirent->cluster = (short_dirent->cluster_hi) | (short_dirent->cluster_hi << 16);

	return (unsigned)(short_dirent + 1) - (unsigned)cluster;
}

/*
 * Dump volume info about each partition
 */
void fat32_dump_part_table()
{
	printf("~~~ Volume ~~~\n");
	printf("vol_lba: 0x%x\t", volume.vol_lba);
	printf("size: %uMB\t", volume.size*volume.sector_size/1024/1024);
	printf("sector_size: %u\t", volume.sector_size);
	printf("cluster_size: %u\n", volume.cluster_size);
	printf("fat_size: %u\t", volume.fat_size);
	printf("num_fats: %u\t", volume.num_fats);
	printf("cluster_lba 0x%x\t", volume.cluster_lba);
	printf("fat_lba 0x%x\t", volume.fat_lba);
	printf("root: 0x%x\n\n", volume.root);

	unsigned char cluster[volume.sector_size * volume.cluster_size];
	unsigned next_dirent = 0;
	struct fat32_dirent_t dirent;
	fat32_get_cluster(volume.root, cluster);
	emmc_dump_block(cluster);
	while ((next_dirent = fat32_readdir(cluster, next_dirent, &dirent))) {
		if (strcmp(dirent.short_name, "")) {
			printf("Long name:  '%s'\n", dirent.long_name);
			printf("Short name: '%s'\n", dirent.short_name);
		}
	}
}

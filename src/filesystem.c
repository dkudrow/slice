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
 * LBA of FAT sector containing cluster
 */
#define FAT_LBA(cluster) (volume.fat_lba + ((cluster) * 4 / 512))

/*
 * LBA of first sector of cluster
 */
#define CLUSTER_LBA(cluster) ((((cluster) - 2) * volume.cluster_size) + volume.cluster_lba)

/*
 * Return true if dirent is a long name directory entry
 */
#define FAT32_IS_LONG(dirent) (READ_1(((uint8_t *)dirent)+11) == 0x0F)

#define VALID_CHAR(c) ( ((c) >= '^' && (c) <= 'z') ||				\
		((c) >= '@' && (c) <= 'Z') || ((c) >= '0' && (c) <= '9') ||	\
		((c) >= '\'' && (c) <= ')') || ((c) >= '#' && (c) <= '&') ||	\
		((c) == '!' ) || ((c) == '-' ) || ((c) == '{' ) ||		\
		((c) == '}' ) || ((c) == '~' ))

#define FAT_MASK 0x0FFFFFFF
#define FAT_TAIL 0x0FFFFFFF
#define FAT_FREE 0x00000000
#define CLUSTER_SIZE (volume.cluster_size * volume.sector_size)

/*
 * On disk layout of MS DOS partition table entry
 */
struct disk_part_t {
	uint8_t boot;
	uint8_t _PAD1[7];
	uint32_t start_lba;
	uint32_t size;
} __attribute__((packed));

/*
 * On disk layout of MBR
 */
struct disk_mbr_t {
	uint8_t _PAD1[446];
	struct disk_part_t part_1;
	struct disk_part_t part_2;
	struct disk_part_t part_3;
	struct disk_part_t part_4;
	uint16_t magic;
} __attribute__((packed));

/*
 * On disk layout of Volume ID/Boot Parameter Block sector
 */
struct disk_bpb_t {
	uint8_t _PAD1[11];
	uint16_t sector_size;		/* size of sector in bytes */
	uint8_t cluster_size;		/* size of cluster in sectors */
	uint16_t reserved_sectors;	/* resrved sectors before FAT */
	uint8_t num_fats;		/* replication of FAT */
	uint8_t _PAD2[19];
	uint32_t fat_size;		/* size of FAT in sectors */
	uint8_t _PAD3[4];
	uint32_t root;			/* cluster containing root directory */
} __attribute__((packed));

/*
 * On disk layout of long directory entry
 */
struct disk_long_dirent_t {
	uint8_t sequence;		/* sequence of entry in long name */
	uint16_t name_1[5];		/* characters 1-5 in entry */
	uint8_t attributes;		/* should always be 0x0F */
	uint8_t type;			/* should always be 0x00 */
	uint8_t checksum;		/* checksum of short name -- ignore */
	uint16_t name_2[6];		/* characters 6-11 in entry */
	uint8_t _PAD[2];
	uint16_t name_3[2];		/* characters 12-13 in entry */
} __attribute__((packed));

/*
 * On disk layout of short directory entry
 */
struct disk_short_dirent_t {
	char name[11];		/* short name in ABCDEFGH.EXT format */
	uint8_t attributes;	/* file attributes */
	uint8_t _PAD;
	uint8_t time_tenths;	/* 0.1s creation time */
	uint16_t creation_time;	/* creation time */
	uint16_t creation_date;	/* creation date */
	uint16_t access_date;	/* last access date */
	uint16_t cluster_hi;	/* hi bytes of first cluster number */
	uint16_t write_time;	/* last write time */
	uint16_t write_date;	/* last write date */
	uint16_t cluster_lo;	/* lo bytes of first cluster number */
	uint32_t size;		/* size of file in bytes */
} __attribute__((packed));

struct vol_t volume;

/*
 * Load a cluster into a buffer
 */
static void fs_get_cluster(unsigned cluster, unsigned char *buf)
{
	int i;
	unsigned lba = CLUSTER_LBA(cluster);
	for (i=0; i<volume.cluster_size; i++)
		emmc_read_block(lba+i, buf+i*volume.sector_size);
}

/*
 * Find the cluster corresponding to an offset
 * Note: returning 0 in case of failure is safe because there is no
 * dirent to pass in for the root directory (which occupies cluster 0)
 */
static unsigned fs_cluster_map(unsigned offset, struct dirent_t *dirent)
{
	unsigned sector[512];
	unsigned cluster_cnt = offset / (volume.cluster_size * 512);
	unsigned cluster_no = dirent->cluster;
	unsigned lba, last_lba = 0;

	if (offset >= dirent->size)
		return 0;

	/* walk FAT cluster chain */
	for (int i=0; i<cluster_cnt; i++) {
		lba = FAT_LBA(cluster_no);
		if (lba != last_lba)
			emmc_read_block(lba, sector);
		cluster_no = sector[cluster_no] & FAT_MASK;
	}

	return cluster_no;
}

/*
 * Read an MS DOS format partition table
 */
void fs_init()
{
	unsigned char sector[512];
	struct disk_mbr_t *mbr = (struct disk_mbr_t *)sector;
	struct disk_part_t *part = &mbr->part_1;
	struct disk_bpb_t *bpb = (struct disk_bpb_t *)sector;

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
unsigned fs_readdir(unsigned char *cluster, unsigned offset,
		struct dirent_t *dirent)
{
	int i, pos, seq;
	struct disk_short_dirent_t *short_dirent;
	struct disk_long_dirent_t *long_dirent =
		(struct disk_long_dirent_t *)(cluster + offset);

	dirent->long_name[0] = '\0';
	dirent->short_name[0] = '\0';

	/* check for empty directory entry */
	if ((cluster + offset)[0] == 0xE5)
		return offset + sizeof(struct disk_short_dirent_t);

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
	short_dirent = (struct disk_short_dirent_t *)long_dirent;
	/*strncpy(&dirent->short_name[0], short_dirent->name, 8);*/
	/*dirent->short_name[8] = '.';*/
	/*strncpy(&dirent->short_name[9], &short_dirent->name[8], 3);*/
	/*dirent->short_name[12] = '\0';*/
	strncpy(&dirent->short_name[0], short_dirent->name, 11);
	dirent->short_name[11] = '\0';

	dirent->cluster = (short_dirent->cluster_lo) | (short_dirent->cluster_hi << 16);
	dirent->size = (unsigned)(short_dirent->size);

	return (unsigned)(short_dirent + 1) - (unsigned)cluster;
}

/*
 * Convert a FAT32 short name into a string
 */
void fs_name_to_str(char *filename, const char *short_name)
{
	int i=0, j=8;

	/* copy base name */
	while (i<8 && short_name[i] > ' ') {
		filename[i] = short_name[i];
		++i;
	}

	/* check for extension */
	if (short_name[8] > ' ')
		filename[i++] = '.';

	/* copy extension */
	while (j<11 && short_name[j] > ' ')
		filename[i++] = short_name[j++];

	filename[i] = '\0';
}

/*
 * Convert a string into a FAT32 short name
 */
int fs_str_to_name(char *short_name, const char *filename)
{
	int i, j;

	memset(short_name, ' ', 11);

	/* copy basename */
	for (i=0; i<8; i++) {
		if (filename[i] == '\0')
			return i == 0 ? -1 : 0;
		if (filename[i] == '.')
			break;
		if (!VALID_CHAR(filename[i]))
			return -1;
		short_name[i] = toupper(filename[i]);
	}

	if (i == 0)
		return -1;

	/* find extension */
	while (filename[i] != '.' && filename[i] != '\0')
		++i;
	while (filename[i] == '.')
		++i;

	/* copy extension */
	for (j=8; j<11; j++) {
		if (filename[i] == '\0')
			return 0;
		if (!VALID_CHAR(filename[i]))
			return -1;
		short_name[j] = toupper(filename[i++]);
	}

	return 0;
}

/*
 * Find directory entry correspending to filename
 */
int fs_lookup(const char *name, struct dirent_t *ret)
{
	unsigned char cluster[CLUSTER_SIZE];
	unsigned offset = 0;
	char short_name[11];

	fs_get_cluster(volume.root, cluster);
	fs_str_to_name(short_name, name);

	while ((offset = fs_readdir(cluster, offset, ret))) {
		if (!strncmp(short_name, ret->short_name, 11))
			return 0;
	}

	return -1;
}

/*
 * Read bytes from file into buffer
 * TODO: set errno
 */
int fs_read(const char *filename, unsigned char* buf, size_t off, size_t count)
{
	struct dirent_t dirent;
	unsigned cluster_no;
	unsigned char cluster[CLUSTER_SIZE]; 
	int start_read, bytes_to_read, last_byte, pos = off;

	if (fs_lookup(filename, &dirent) != 0)
		return -1; /* FIXME -- no such file */

	/* perform read for each cluster */
	last_byte = MIN(off+count, dirent.size);
	while (pos < last_byte) {
		cluster_no = fs_cluster_map(pos, &dirent);
		if (cluster_no == 0)
			return pos - off;
		fs_get_cluster(cluster_no, cluster);
		start_read = pos % CLUSTER_SIZE;
		bytes_to_read = MIN(CLUSTER_SIZE, last_byte - pos);
		memcpy(&buf[pos-off], &cluster[start_read], bytes_to_read);
		pos += bytes_to_read;
	}

	return pos - off;
}

/*
 * Dump volume info about each partition
 */
void fs_dump_part_table()
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

	/* read root dir */
	unsigned char cluster[volume.sector_size * volume.cluster_size];
	unsigned next_dirent = 0;
	struct dirent_t dirent;
	fs_get_cluster(volume.root, cluster);
	emmc_dump_block(cluster);
	while ((next_dirent = fs_readdir(cluster, next_dirent, &dirent))) {
		if (strcmp(dirent.short_name, "")) {
			printf("Long name:  '%s'\n", dirent.long_name);
			printf("Short name: '%s'\n", dirent.short_name);
		}
	}
}

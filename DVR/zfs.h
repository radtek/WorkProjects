#ifndef _ZFS_H
#define _ZFS_H

#include "zfs_type.h"

#pragma pack(push, 1)

typedef struct zio_cksum {
	uint64_t	word[4];
} zio_cksum_t;

typedef struct _dva {
	uint64_t	alloc_size:24;				/* allocated size (including RAID-Z parity and gang block headers) */
	uint64_t	grid:8;						/* RAID-Z layout information (reserved for future use) */
	uint64_t	vdev_id:32;					/* virtual device ID */
	uint64_t	offset:31;					/* offset in sectors (512 bytes) */
	uint64_t	gang_flag:1;				/* gang block indicator */
} dva_t;

#define	SPA_BLKPTRSHIFT		7				/* blkptr_t is 128 bytes	*/

typedef struct _blk_props {
	uint64_t	logical_size:16;			/* logical size */
	uint64_t	physical_size:16;			/* physical size (after compression) */
	uint64_t	compression:7;				/* compression function */
	uint64_t	embedded:1;					/* blkptr_t contains embedded data (see below) blk_props_emb_t */
	uint64_t	checksum:8;					/* checksum function */
	uint64_t	type:8;						/* DMU object type */
	uint64_t	level:5;					/* level of indirection */
	uint64_t	encryption:1;				/* encryption (on version 30, which is not supported) */
	uint64_t	dedup:1;					/* deduplication */
	uint64_t	byteorder:1;				/* byteorder (endianness)*/
} blk_props_t;

typedef struct _blk_props_emb {
	uint64_t	logical_size:25;			/* logical size */
	uint64_t	physical_size:7;			/* physical size (after compression) */
	uint64_t	compression:7;				/* compression function */
	uint64_t	embedded:1;					/* set to one */
	uint64_t	etype:8;					/* how to interpret embedded data (BP_EMBEDDED_TYPE_*) */
	uint64_t	type:8;						/* DMU object type */
	uint64_t	level:5;					/* level of indirection */
	uint64_t	encryption:1;				/* encryption (on version 30, which is not supported) */
	uint64_t	dedup:1;					/* deduplication */
	uint64_t	byteorder:1;				/* byteorder (endianness)*/
} blk_props_emb_t;

#define	SPA_DVAS_PER_BP		3				/* Number of DVAs in a bp */

typedef struct blkptr {
	dva_t		dva[SPA_DVAS_PER_BP];		/* Data Virtual Addresses */
	blk_props_t	props;						/* size, compression, type, etc */
	uint64_t	padding[2];					/* Extra space for the future */
	uint64_t	phys_birth;					/* txg of block allocation; zero if same as logical birth txg */
	uint64_t	log_birth;					/* transaction group in which the block was logically born */
	uint64_t	fill;						/* fill count */
	zio_cksum_t	checksum;					/* 256-bit checksum */
} blkptr_t;

#define	UBERBLOCK_MAGIC		0x00bab10c		/* oo-ba-bloc! */
#define	UBERBLOCK_SHIFT		10				/* up to 1K	*/

typedef struct uberblock {
	uint64_t	magic;						/* UBERBLOCK_MAGIC */
	uint64_t	version;					/* SPA_VERSION */
	uint64_t	txg;						/* txg of last sync */
	uint64_t	guid_sum;					/* sum of all vdev guids */
	uint64_t	timestamp;					/* UTC time of last sync */
	blkptr_t	rootbp;						/* MOS objset_phys_t */

	/* highest SPA_VERSION supported by software that wrote this txg */
	uint64_t	software_version;
} uberblock_t;

#define	DNODE_SHIFT			9				/* 512 bytes */
#define	DNODE_SIZE			(1 << DNODE_SHIFT)
#define	DNODE_CORE_SIZE		64				/* 64 bytes for dnode sans blkptrs */
#define	DN_MAX_BONUSLEN	(DNODE_SIZE - DNODE_CORE_SIZE - (1 << SPA_BLKPTRSHIFT))

typedef struct dnode_phys {
	uint8_t		type;						/* dmu_object_type_t */
	uint8_t		ind_blk_shift;				/* ln2(indirect block size) */
	uint8_t		nlevels;					/* 1=dn_blkptr->data blocks */
	uint8_t		nbl_kptr;					/* length of dn_blkptr */
	uint8_t		bonus_type;					/* type of data in bonus buffer */
	uint8_t		checksum;					/* ZIO_CHECKSUM type */
	uint8_t		compress;					/* ZIO_COMPRESS type */
	uint8_t		flags;						/* DNODE_FLAG_* */
	uint16_t	data_blk_sz_sec;			/* data block size in 512b sectors */
	uint16_t	bonus_len;					/* length of dn_bonus */
	uint8_t		pad2[4];

	/* accounting is protected by dn_dirty_mtx */
	uint64_t	max_blk_id;					/* largest allocated block ID */
	uint64_t	used;						/* bytes (or sectors) of disk space */
	uint64_t	pad3[4];

	/*
	 * The tail region is 448 bytes, and there are three ways to
	 * look at it.
	 *
	 * 0       64      128     192     256     320     384     448 (offset)
	 * +---------------+---------------+---------------+-------+
	 * | dn_blkptr[0]  | dn_blkptr[1]  | dn_blkptr[2]  | /     |
	 * +---------------+---------------+---------------+-------+
	 * | dn_blkptr[0]  | dn_bonus[0..319]                      |
	 * +---------------+-----------------------+---------------+
	 * | dn_blkptr[0]  | /                     | dn_spill      |
	 * +---------------+-----------------------+---------------+
	 */
	union {
		blkptr_t		blk_ptr[1+DN_MAX_BONUSLEN/sizeof(blkptr_t)];
		struct {
			blkptr_t	ignore1;
			uint8_t		bonus[DN_MAX_BONUSLEN];
		};
		struct {
			blkptr_t	ignore2;
			uint8_t		ignore3[DN_MAX_BONUSLEN-sizeof(blkptr_t)];
			blkptr_t	spill;
		};
	};
} dnode_phys_t;


#pragma pack(pop)

#endif // _ZFS_H
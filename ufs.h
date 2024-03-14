#ifndef __ufs_h__
#define __ufs_h__

#define UFS_DIRECTORY (0)
#define UFS_REGULAR_FILE (1)

#define UFS_BLOCK_SIZE (4096)

#define DIRECT_PTRS (30)

typedef struct {
    int type;   // MFS_DIRECTORY or MFS_REGULAR
    int size;   // bytes
    unsigned int direct[DIRECT_PTRS];
} inode_t;

typedef struct {
    char name[28];  // up to 28 bytes of name in directory (including \0)
    int  inum;      // inode number of entry (-1 means entry not used)
} dir_ent_t;

// presumed: block 0 is the super block
typedef struct __super {
    int inode_bitmap_addr; // block address (in blocks)
    int inode_bitmap_len;  // in blocks
    int data_bitmap_addr;  // block address (in blocks)
    int data_bitmap_len;   // in blocks
    int inode_region_addr; // block address (in blocks)
    int inode_region_len;  // in blocks
    int data_region_addr;  // block address (in blocks)
    int data_region_len;   // in blocks
    int num_inodes;        // just the number of inodes
    int num_data;          // and data blocks...
} super_t;

typedef unsigned int* bitmap_t;

typedef struct __ufs {
	int fd;
	super_t s;
	bitmap_t inode_bp; // inode bitmap
	int inode_bp_sz; // inode bitmap size (size of inode_bp array)
	bitmap_t data_bp; // data bitmap
        int data_bp_sz; // data bitmap size
	inode_t *inodes;

	//recording dirty parts to make writes convenient
	//u would think its for caching writes or smth, but no, this
	//system aint even close to efficient lol
	bitmap_t dirty_inode_bp;
	bitmap_t dirty_data_bp;
} ufs;

typedef struct __dir_block_t {
	dir_ent_t entries[128];
} dir_block_t;

ufs* ufs_init(char *fname);
int ufs_lookup(ufs *nfs, int pinum, char *name);
int ufs_creat(ufs *nfs, int pinum, int type, char *name);
int ufs_write(ufs *nfs, int inum, char *buf, int offset, int nbytes);
int ufs_read(ufs *nfs, int inum, char *buffer, int offset, int nbytes);
int ufs_unlink(ufs *nfs, int pinum, char *name);
void ufs_clean(ufs *nfs);

#endif // __ufs_h__

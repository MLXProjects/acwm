#ifndef __partmgr_h__
#define __partmgr_h__

#define PARTMGR_FSTAB_AREC		0x1
#define PARTMGR_FSTAB_TWRP		0x2
#define PARTMGR_FSTAB_PROC		0x4

typedef struct {
	char *device;
	char *mountpoint;
	char fs[16];
	char *mountflags;
	char *voldflags;
	//char name[32];
	byte formattable;
	byte mountable;
	byte mounted;
} PARTITION, * PARTITIONP;

typedef struct {
	PARTITIONP *parts;
	int partn;
	byte fstab_type;
} PARTMGR_INFO, * PARTMGR_INFOP;

extern PARTMGR_INFOP partmgr();
extern void part_cleanup();
extern byte part_readfstab();
extern byte part_readproc(byte add_missing);
extern PARTITIONP part_get(char *blockdevice, char *mountpoint);
extern PARTITIONP part_get_atindex(int index, byte mountable, byte formattable);
extern byte part_mount(PARTITIONP part, char *blockdevice);
extern byte part_unmount(PARTITIONP part, char *blockdevice);
extern byte part_format(PARTITIONP part, char *blockdevice);

#endif /*__partmgr_h__*/
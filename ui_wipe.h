#ifndef __ui_wipe_h__
#define __ui_wipe_h__

/* flags for wiping */
#define WIPE_GENERIC		0x0
#define WIPE_MANUAL			0x1
#define WIPE_DALVIK			0x2
#define WIPE_CACHE			0x4
#define WIPE_DATA			0x8

/* ui_wipe functions */
extern void uiwipe_setcallback(void *cb);
extern void uiwipe_setparts(byte parts_flag, PARTITIONP part1, PARTITIONP part2);

#endif /*__ui_wipe_h__*/
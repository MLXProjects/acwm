#include "recovery.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static PARTMGR_INFO _partmgr;

PARTMGR_INFOP partmgr(){
	return &_partmgr;
}

void part_cleanup(){
	if (!_partmgr.parts) return;
	int i;
	for (i=_partmgr.partn-1; i>=0; i--){
		if (_partmgr.parts[i]){
			
			free(_partmgr.parts[i]);
		}
	}
	free(_partmgr.parts);
	_partmgr.parts = NULL;
	_partmgr.partn = 0;
}

byte _part_add(PARTITIONP part){
	/* reallocate list to fit new partition */
	//alog("reallocating list [%d]->[%d]", _partmgr.partn, _partmgr.partn+1);
	PARTITIONP *parts = realloc(_partmgr.parts, sizeof(PARTITIONP)*(_partmgr.partn+1));
	if (!parts){
		alog("failed to reallocate partition list");
		return 0;
	}
	/* add to list at last index */
	parts[_partmgr.partn] = part;
	/* update list & count */
	_partmgr.parts = parts;
	_partmgr.partn++;
	return 1;
}

byte _part_parsefstab(char *buffer){
	/* Android recovery fstab format:
	 * <blockdevice> <mountpoint> <filesystem> <mountflags> <voldflags>
	 */
	/* cleanup any previous data */
	part_cleanup();
	/* declare later use variables */
	PARTITIONP part;
	int word_index=0, word_len;
	alog("parsing fstab");
	/* break string into lines */
	char *curLine = buffer, *curWord, *nextLine, *nextWord, *newmount, *endnewmount;
	while(curLine){
		nextLine = strchr(curLine, '\n');
		/* temporarily terminate current line */
		if (nextLine) *nextLine = '\0';
		/* skip trailing spaces & ignore comment/empty lines */
		while(*curLine==' ') curLine++;
		if (curLine[0]!='#' && curLine[0]!='\0'){
			/* grow partition list */
			/* allocate partition data */
			//alog("allocating partition #%d", _partmgr.partn);
			part = malloc(sizeof(PARTITION));
			if (!part){
				alog("failed to allocate partition");
				return 0;
			}
			part->mountable = 0;
			part->mounted = 0;
			part->formattable = 1;
			/* break line into words */
			curWord = curLine;
			while(curWord){
				nextWord = strchr(curWord, ' ');
				if (nextWord) *nextWord = '\0';
				/* ignore empty lines */
				if (curWord[0]!='\0'){
					if (word_index>4) word_index=0;
					/* save word at correct place */
					switch (word_index){
						case 0:{ /* blockdevice */
							/* auto allocate buffer and resolve symlinks (e.g. /dev/block/bootdevice...) */
							#ifdef _WIN32
							#define realpath(x,y) strdup(x)
							#endif
							char *resolved_path = realpath(curWord, NULL);
							#ifdef _WIN32
							#undef realpath
							#endif
							if (resolved_path){
								part->device = resolved_path;
							}
							/* if allocating fails, retry with original text & length */
							else {
								part->device = strdup(curWord);
								if (!part->device){
									alog("failed to allocate partition device");
									break;
								}
							}
						} break;
						case 1:{ /* mountpoint */
							part->mountpoint = strdup(curWord);
							if (!part->mountpoint){
								alog("failed to allocate partition mountpoint");
								break;
							}
						} break;
						case 2:{ /* filesystem */
							word_len = strlen(curWord);
							if (!strcmp(curWord,"ext4") || !strcmp(curWord,"vfat") ||
									!strcmp(curWord,"exfat") || !strcmp(curWord,"auto"))
								part->mountable = 1;
							else if (!strcmp(curWord,"swap")) part->formattable=0;
							strncpy(part->fs, curWord, word_len);
							part->fs[word_len]='\0';
						} break;
						case 3:{ /* mountflags */
							part->mountflags = strdup(curWord);
							if (!part->mountflags){
								alog("failed to allocate partition mountflags");
								break;
							}
						} break;
						case 4:{ /* voldflags */
							/* check for auto mountpoint */
							if (newmount=strstr(curWord, "voldmanaged=")){
								newmount+=12;
								endnewmount=strchr(newmount, ':');
								if (!endnewmount) endnewmount=strchr(newmount, ',');
								if (endnewmount){
									word_len = (endnewmount-newmount)+1;
									char *mountpoint=malloc(sizeof(char)*(word_len+1));
									if (mountpoint){
										free(part->mountpoint);
										snprintf(mountpoint, word_len, "/%s", newmount);
										mountpoint[word_len]='\0';
										part->mountpoint=mountpoint;
									}
									else {
										alog("failed to reallocate old mountpoint");
									}
								}
							}
							part->voldflags = strdup(curWord);
							if (!part->voldflags){
								alog("failed to allocate partition voldflags");
								break;
							}
						} break;
					}
					word_index++;
				}
				if (nextWord) *nextWord = ' ';
				curWord = nextWord?(nextWord+1):NULL;
				/* skip trailing spaces for next line */
				if (curWord){
					while(*curWord==' ' || *curWord=='\t') curWord++;
				}
			}
			/* update parts list & dump partition data */
			alog("PART dev=%s, mount=%s, fs=%s, flags=%s, vold=%s\n",
				part->device, part->mountpoint,
				part->fs, part->mountflags,
				part->voldflags);
			if (!_part_add(part)){
				alog("failed to add partition");
				free(part);
				return 0;
			}
		}
		/* restore newline char */
		if (nextLine) *nextLine = '\n';
		curLine = nextLine?(nextLine+1):NULL;
	}
	return 1;
}

byte _part_parseproc(char *buffer, byte add_missing){
	/* /proc/mounts format:
	 * <blockdevice> <mountpoint> <filesystem> <mountflags> <whoknows1> <whoknows2> 
	 */
	
	/* declare later use variables */
	int word_index=0, word_len;
	alog("parsing proc");
	/* break string into lines */
	char *curLine = buffer, *nextLine, *firstSpace;
	PARTITIONP part;
	while(curLine){
		nextLine = strchr(curLine, '\n');
		/* temporarily terminate current line */
		if (nextLine) *nextLine = '\0';
		/* only care about absolute paths, they're always block devices on /proc/mounts */
		if (curLine[0]=='/'){
			/* find first space */
			firstSpace = strchr(curLine, ' ');
			/* temporarily terminate current word */
			if (firstSpace) *firstSpace = '\0';
			/* use blockdevice as identifier */
			/* TODO: check using mountpoint instead? */
			part = part_get(curLine, NULL);
			if (part && !part->mounted){
				alog("setting %s as mounted", part->device);
				part->mounted = 1;
			}
			else if (!part){
				/* TODO: add missing partitions to manager if needed (check flag) */
			}
			if (firstSpace) *firstSpace = ' ';
		}
		/* restore newline char */
		if (nextLine) *nextLine = '\n';
		curLine = nextLine?(nextLine+1):NULL;
	}
	return 1;
}

byte part_readproc(byte add_missing){
	LIBAROMA_STREAMP proc_file = libaroma_stream_file("/proc/mounts");
	if (!proc_file){
		alog("failed to open /proc/mounts");
		return 0;
	}
	byte parse_done;
	/* save file contents to buffer */
	int file_sz = proc_file->size;
	char *file_data = proc_file->data,
		*file_buffer = malloc(file_sz+1);
	if (!file_buffer){
		alog("failed to allocate buffer for file parsing");
		libaroma_stream_close(proc_file);
		return 0;
	}
	memcpy(file_buffer, file_data, file_sz);
	file_buffer[file_sz]='\0';
	/* release file */
	libaroma_stream_close(proc_file);
	/* parse proc */
	parse_done = _part_parseproc(file_buffer, add_missing);
	if (!parse_done){
		alog("failed to parse proc");
	}
	else alog("parsing done");
	/* free buffer */
	free(file_buffer);
	return parse_done;
}

byte part_readfstab(){
	/* load fstab from hardcoded path */
	LIBAROMA_STREAMP fstab_file = libaroma_stream_file("/etc/recovery.fstab");
	/* if failed maybe we're on desktop, try testing file */
	if (!fstab_file) fstab_file = libaroma_stream_mzip(res_zip, "fstabs/recovery.fstab");
	/* check for file opened & meaningful size */
	if (!fstab_file){
		alog("recovery.fstab missing, fallback to /proc/mounts");
		/* load /proc/mounts as fstab */
		return part_readproc(1);
	}
	if (fstab_file->size<16){
		alog("invalid recovery.fstab? fallback to /proc/mounts");
		libaroma_stream_close(fstab_file);
		/* load /proc/mounts as fstab */
		return part_readproc(1);
	}
	int file_sz = fstab_file->size;
	char *file_data = fstab_file->data;
	byte hasbom = 0;
	/* check UTF-8 BOM */
	if (file_data[0]==0xEF &&
			file_data[1]==0xBB &&
			file_data[2]==0xBF){
		/* skip BOM */
		file_sz -= 3;
		file_data += 3;
	}
	byte parse_done;
	/* save file contents to buffer */
	char *file_buffer = malloc(file_sz+1);
	if (!file_buffer){
		alog("failed to allocate buffer for file parsing");
		libaroma_stream_close(fstab_file);
		return 0;
	}
	memcpy(file_buffer, file_data, file_sz);
	file_buffer[file_sz]='\0';
	/* release file */
	libaroma_stream_close(fstab_file);
	/* parse fstab */
	parse_done = _part_parsefstab(file_buffer);
	if (!parse_done){
		alog("failed to parse fstab");
	}
	else alog("parsing done");
	/* free buffer */
	free(file_buffer);
	return parse_done;
}

PARTITIONP part_get(char *blockdevice, char *mountpoint){
	if (!blockdevice) return NULL;
	int i;
	PARTITIONP part;
	for (i=_partmgr.partn-1; i>=0; i--){
		part = _partmgr.parts[i];
		if ((blockdevice && !strcmp(blockdevice, part->device)) ||
			(mountpoint && !strcmp(mountpoint, part->mountpoint))){
			return part;
		}
	}
	return NULL;
}

PARTITIONP part_get_atindex(int index, byte mountable, byte formattable){
	if (index < 0 || index >= _partmgr.partn) return NULL;
	int i, mountable_index = 0, formattable_index = 0;
	PARTITIONP part;
	for (i=0; i<_partmgr.partn; i++){
		part = _partmgr.parts[i];
		if (mountable && part->mountable){
			if (mountable_index == index) return part;
			mountable_index++;
		}
		else if (formattable && part->formattable){
			if (formattable_index == index) return part;
			formattable_index++;
		}
	}
	return NULL;
}

byte part_mount(PARTITIONP part, char *blockdevice){
	if (!part) part = part_get(blockdevice, NULL);
	if (!part){
		alog("invalid partition");
		return 0;
	}
	if (part->mounted){
		alog("partition already mounted");
		return 0;
	}
	/* create mountpoint just in case */
	#ifdef _WIN32
	mkdir(part->mountpoint);
	#else
	mkdir(part->mountpoint, S_IRWXU | S_IRWXG | S_IRWXO);
	#endif
	/* run mount command lol */
	char command[256];
	snprintf(command, 256, "mount -t %s %s %s", part->fs, part->device, part->mountpoint);
	alog("executing \"%s\"", command);
	system(command);
	return 1;
}

byte part_unmount(PARTITIONP part, char *blockdevice){
	if (!part) part = part_get(blockdevice, NULL);
	if (!part){
		alog("invalid partition");
		return 0;
	}
	if (!part->mounted){
		alog("partition not mounted");
		return 0;
	}
	/* run unmount command lol */
	char command[256];
	snprintf(command, 256, "umount %s", part->mountpoint);
	alog("executing \"%s\"", command);
	system(command);
	return 1;
}

byte part_format(PARTITIONP part, char *blockdevice){
	if (!part) part = part_get(blockdevice, NULL);
	if (!part){
		alog("invalid partition");
		return 0;
	}
	/* safety checks */
	if (part->mounted){
		if (!part_unmount(part, NULL)){
			alog("failed to unmount partition before format");
			return 0;
		}
	}
	if (!part->formattable){
		alog("partition not formattable");
		return 0;
	}
	/* run format command lol */
	char command[256];
	if (!strcmp(part->fs,"ext4"))
		snprintf(command, 256, "make_ext4fs -a %s %s", part->mountpoint, part->device);
	else if (!strcmp(part->fs,"vfat"))
		snprintf(command, 256, "newfs_msdos -F 32 -O android -c 8 %s", part->device);
	else if (!strcmp(part->fs,"exfat"))
		snprintf(command, 256, "mkfs.exfat %s", part->device);
	else {
		alog("unable to find program to format filesystem \"%s\"", part->fs);
		return 0;
	}
	alog("executing \"%s\"", command);
	system(command);
	return 1;
}

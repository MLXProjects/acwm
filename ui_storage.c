#include "recovery.h"

static UI_MENU uistorage_menu;

void uistorage_free(){
	/* release item strings */
	int i;
	/* loop until itemn-1 to not free back item */
	for (i=0; i<uistorage_menu.itemn-1; i++){
		free(uistorage_menu.items[i]->text);
	}
	uistorage_menu.itemn = 0;
}

byte uistorage_load(){
	uistorage_menu.freecb = &uistorage_free;
	/* get partition list & create items */
	alog("uistorage_load reading fstab");
	part_readfstab();
	alog("uistorage_load reading proc");
	part_readproc(0);
	int i, itemn=0, item_len;
	/* get localized strings beforehand */
	char *localized_mount = libaroma_lang_get("storage.mount"),
		*localized_unmount = libaroma_lang_get("storage.unmount"),
		*localized_format = libaroma_lang_get("storage.format");
	int mountprefix_len = strlen(localized_mount)+1,
		unmountprefix_len = strlen(localized_unmount)+1,
		formatprefix_len = strlen(localized_format)+1,
		mountable_count=0;
	/* create item list */
	char **uistorage_items=NULL, **temp_list;
	PARTITIONP part;
	/* iterate partition list twice; first to find mountable partitions, then
	 * to find formattable ones.*/
	for (i=0; i<partmgr()->partn; i++){
		part = partmgr()->parts[i];
		if (part->mountable){
			temp_list = realloc(uistorage_items, sizeof(char*)*(itemn+1));
			if (!temp_list){
				alog("failed to reallocate storage items list");
				if (itemn) free(uistorage_items);
				return 0;
			}
			item_len = ((part->mounted)?unmountprefix_len:mountprefix_len)+strlen(part->mountpoint)+1;
			temp_list[itemn] = malloc(sizeof(char)*(item_len+1));
			if (!temp_list[itemn]){
				alog("failed to allocate storage item text");
				free(temp_list);
				return 0;
			}
			snprintf(temp_list[itemn], item_len, "%s %s", 
				(part->mounted)?localized_unmount:localized_mount, part->mountpoint);
			temp_list[itemn][item_len] = '\0';
			uistorage_items = temp_list;
			itemn++;
			mountable_count++;
		}
	}
	for (i=0; i<partmgr()->partn; i++){
		part = partmgr()->parts[i];
		if (part->formattable){
			temp_list = realloc(uistorage_items, sizeof(char*)*(itemn+1));
			if (!temp_list){
				alog("failed to reallocate storage items list");
				if (itemn) free(uistorage_items);
				return 0;
			}
			item_len = formatprefix_len+strlen(part->mountpoint)+1;
			temp_list[itemn] = malloc(sizeof(char)*(item_len+1));
			if (!temp_list[itemn]){
				alog("failed to allocate storage item text");
				free(temp_list);
				return 0;
			}
			snprintf(temp_list[itemn], item_len, "%s %s", localized_format, part->mountpoint);
			temp_list[itemn][item_len] = '\0';
			uistorage_items = temp_list;
			itemn++;
		}
	}
	if (!uitools_loadmenu(&uistorage_menu, libaroma_lang_get("storage.title"), itemn, MENU_STOR, uistorage_items, 1)){
		uitools_freemenu(&uistorage_menu);
		return 0;
	}
	/* fix item ids */
	for (i=0; i<mountable_count; i++){
		uistorage_menu.items[i]->id = UISTOR_MOUNT+i;
	}
	for (i=mountable_count; i<itemn; i++){
		uistorage_menu.items[i]->id = UISTOR_FORMAT+i-mountable_count;
	}
	return 1;
}

byte uistorage_ui_thread(){
	if (ui_update){
		uitools_drawbg();
		uitools_drawmenu(&uistorage_menu);
		ui_update=0;
		return 1;
	}
	return 0;
}

byte uistorage_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->state!=1 && msg->msg != LIBAROMA_MSG_EXIT) return 0;
	uitools_volarrows(msg);
	/* check message */
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:{
			uitools_freemenu(&uistorage_menu);
			libaroma_wm_set_ui_thread(NULL);
		} return 1;
		case LIBAROMA_MSG_USR(UI_MSG_ACTIVE):{
			ui_update = 1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLDOWN:{
			if (uistorage_menu.selected>uistorage_menu.itemn-2) uistorage_menu.selected=0;
			else uistorage_menu.selected++;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLUP:{
			if (uistorage_menu.selected<1) uistorage_menu.selected=uistorage_menu.itemn-1;
			else uistorage_menu.selected--;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			int id = uistorage_menu.items[uistorage_menu.selected]->id,
				last_selected = uistorage_menu.selected;
			alog("selected item[%d]", id);
			if (id >= UISTOR_MOUNT){
				PARTITIONP part = part_get_atindex(id-UISTOR_MOUNT, 1, 0);
				if (!part){
					alog("failed to find partition at %d", id-UISTOR_MOUNT);
					break;
				}
				alog("selected %smount %s", (part->mounted)?"un":"", part->mountpoint);
				/* un/mount partition & reload current menu */
				alog("freeing menu");
				uitools_freemenu(&uistorage_menu);
				if (part->mounted) part_unmount(part, NULL);
				else part_mount(part, NULL);
				alog("reloading menu");
				uistorage_load();
				uistorage_menu.selected = last_selected;
				alog_ui(libaroma_lang_get("generic.done"));
				libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
			}
			else if (id >= UISTOR_FORMAT){
				PARTITIONP part = part_get_atindex(id-UISTOR_FORMAT, 0, 1);
				if (!part){
					alog("failed to find partition at %d", id-UISTOR_FORMAT);
					break;
				}
				alog("selected format %s", part->mountpoint);
				/* jump to confirmation screen */
				uiwipe_setparts(WIPE_MANUAL, part, NULL);
				libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				/* release current menu */
				alog("freeing menu");
				uitools_freemenu(&uistorage_menu);
			}
			else if (id == GENE_BACK){
					libaroma_wm_set_message_handler(&uimain_msg_handler);
					libaroma_wm_set_ui_thread(uimain_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
					uitools_freemenu(&uistorage_menu);
			}
		} return 1;
	}
	//alog("unhandled message %d", msg->msg);
	return 0;
}
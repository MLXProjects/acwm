#include "recovery.h"

static UI_MENU uiwipe_menu;
static void *parent_handler;
static void *parent_thread;
static void (*uiwipe_callback)();
static byte uiwipe_partsflag;
static PARTITIONP uiwipe_parts[2];
static char *uiwipe_items[]={
	"generic.no",
	"wipe.yes"
};

void uiwipe_free(){
	/* release preallocated title */
	free(uiwipe_menu.title);
}

byte uiwipe_load(){
	uiwipe_menu.freecb = &uiwipe_free;
	/* backup current handler & thread */
	parent_handler = libaroma_wm()->message_handler;
	parent_thread = libaroma_wm()->ui_thread;
	/* build menu title */
	char *localized_title,
		*localized_warning = libaroma_lang_get("wipe.warning");
	/* initial title length = warning length + newline & 2 spaces */
	int title_len = strlen(localized_warning) + 3;
	byte title_haspart = 0;
	if (uiwipe_partsflag&WIPE_DATA){
		localized_title = libaroma_lang_get("wipe.title_data");
	}
	else if (uiwipe_partsflag&WIPE_MANUAL){
		localized_title = libaroma_lang_get("wipe.title_format");
		/* title length += mountpoint length + space, hypen & space */
		title_len += strlen(uiwipe_parts[0]->mountpoint) + 3;
		title_haspart = 1;
	}
	else {
		localized_title = libaroma_lang_get("wipe.title");
	}
	title_len += strlen(localized_title);
	char *uiwipe_title = malloc(title_len + 1);
	if (!uiwipe_title){
		alog("failed to allocate wipe title");
		return 0;
	}
	snprintf(uiwipe_title, title_len + 1, "%s%s%s\n  %s",
		(title_haspart)?uiwipe_parts[0]->mountpoint:"", (title_haspart)?" - ":"",
		localized_title, localized_warning);
	if (!uitools_loadmenu(&uiwipe_menu, uiwipe_title, UIWIPE_ITEMCOUNT, MENU_GENE, uiwipe_items, 0)){
		uitools_freemenu(&uiwipe_menu);
		return 0;
	}
	return 1;
}

void uiwipe_setcallback(void *cb){
	uiwipe_callback = cb;
}

void uiwipe_setparts(byte parts_flag, PARTITIONP part1, PARTITIONP part2){
	if (parts_flag&WIPE_DATA)
		uiwipe_items[1]="wipe.yes_all";
	else if (parts_flag&WIPE_CACHE)
		uiwipe_items[1]="wipe.yes_cache";
	else if (parts_flag&WIPE_DALVIK)
		uiwipe_items[1]="wipe.yes_dalvik";
	else if (parts_flag&WIPE_MANUAL)
		uiwipe_items[1]="wipe.yes_format";
	uiwipe_partsflag = parts_flag;
	uiwipe_parts[0]=part1;
	uiwipe_parts[1]=part2;
}

byte uiwipe_ui_thread(){
	if (ui_update){
		uitools_drawbg();
		uitools_drawmenu(&uiwipe_menu);
		ui_update=0;
		return 1;
	}
	return 0;
}

byte uiwipe_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->state!=1 && msg->msg != LIBAROMA_MSG_EXIT) return 0;
	uitools_volarrows(msg);
	/* check message */
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:{
			uitools_freemenu(&uiwipe_menu);
			libaroma_wm_set_ui_thread(NULL);
		} return 1;
		case LIBAROMA_MSG_USR(UI_MSG_ACTIVE):{
			uiwipe_menu.selected = 0;
			ui_update = 1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLDOWN:{
			if (uiwipe_menu.selected>uiwipe_menu.itemn-2) uiwipe_menu.selected=0;
			else uiwipe_menu.selected++;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLUP:{
			if (uiwipe_menu.selected<1) uiwipe_menu.selected=uiwipe_menu.itemn-1;
			else uiwipe_menu.selected--;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			switch (uiwipe_menu.items[uiwipe_menu.selected]->id){
				case GENE_NO:{
					libaroma_wm_set_message_handler(parent_handler);
					libaroma_wm_set_ui_thread(parent_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
					uitools_freemenu(&uiwipe_menu);
				} break;
				case GENE_YES:{
					if (uiwipe_callback){
						/* run custom function instead */
						uiwipe_callback();
						/* reset callback */
						uiwipe_callback = NULL;
					}
					else {
						/* format partitions */
						int i;
						char format_msg[256];
						for (i=0; i<2; i++){
							if (uiwipe_parts[i]){
								snprintf(format_msg, 256, "%s %s...", 
									libaroma_lang_get("wipe.formatting"), uiwipe_parts[i]->mountpoint);
								alog_ui(format_msg);
								part_format(uiwipe_parts[i], NULL);
							}
						}
					}
					/* restore previous message handler & ui thread */
					libaroma_wm_set_message_handler(parent_handler);
					libaroma_wm_set_ui_thread(parent_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
					/* release this menu */
					uitools_freemenu(&uiwipe_menu);
				} break;
			}
		} return 1;
	}
	//alog("unhandled message %d", msg->msg);
	return 0;
}
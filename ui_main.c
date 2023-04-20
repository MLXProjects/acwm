#include "recovery.h"

static UI_MENU uimain_menu;

static char *uimain_items[]={
	"main.reboot",
	"main.install",
	"main.wipedata",
	"main.wipecache",
	"main.backup",
	"main.storage",
	"main.advanced"
};

byte uimain_load(){
	alog_ui(libaroma_lang_get("main.title"));
	if (!uitools_loadmenu(&uimain_menu, libaroma_lang_get("main.title"), UIMAIN_ITEMCOUNT, MENU_MAIN, uimain_items, 0)){
		uitools_freemenu(&uimain_menu);
		return 0;
	}
	return 1;
}

byte uimain_ui_thread(){
	if (ui_update){
		uitools_drawbg();
		uitools_drawmenu(&uimain_menu);
		ui_update=0;
		return 1;
	}
	return 0;
}

byte uimain_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	/* if message is not exit and state is not 1, ignore it */
	if (msg->state!=1 && msg->msg != LIBAROMA_MSG_EXIT) return 0;
	uitools_volarrows(msg);
	/* check message */
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:{
			uitools_freemenu(&uimain_menu);
			libaroma_wm_set_ui_thread(NULL);
		} return 1;
		case LIBAROMA_MSG_USR(UI_MSG_ACTIVE):{
			uimain_menu.selected = 0;
			ui_update = 1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLDOWN:{
			if (uimain_menu.selected>uimain_menu.itemn-2) uimain_menu.selected=0;
			else uimain_menu.selected++;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLUP:{
			if (uimain_menu.selected<1) uimain_menu.selected=uimain_menu.itemn-1;
			else uimain_menu.selected--;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			switch (uimain_menu.items[uimain_menu.selected]->id){
				case MAIN_REBOOT:{
					libaroma_msg_post(LIBAROMA_MSG_EXIT,0,0,0,0,NULL);
				} break;
				case MAIN_INSTALL:{
					libaroma_wm_set_message_handler(&uiinstall_msg_handler);
					libaroma_wm_set_ui_thread(uiinstall_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
				case MAIN_WIPEDATA:{
					uiwipe_setparts(WIPE_DATA|WIPE_CACHE, 
						part_get(NULL, "/data"), part_get(NULL, "/cache"));
					if (!uiwipe_load()){
						alog("failed to load wipe ui");
						break;
					}
					libaroma_wm_set_message_handler(&uiwipe_msg_handler);
					libaroma_wm_set_ui_thread(uiwipe_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
				case MAIN_WIPECACHE:{
					uiwipe_setparts(WIPE_CACHE, part_get(NULL, "/cache"), NULL);
					if (!uiwipe_load()){
						alog("failed to load wipe ui");
						break;
					}
					libaroma_wm_set_message_handler(&uiwipe_msg_handler);
					libaroma_wm_set_ui_thread(uiwipe_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
				case MAIN_BACKREST:{
					libaroma_wm_set_message_handler(&uibackup_msg_handler);
					libaroma_wm_set_ui_thread(uibackup_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
				case MAIN_MOUNTS:{
					if (!uistorage_load()){
						alog("failed to load storage ui");
						break;
					}
					libaroma_wm_set_message_handler(&uistorage_msg_handler);
					libaroma_wm_set_ui_thread(uistorage_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
				case MAIN_ADVANCED:{
					libaroma_wm_set_message_handler(&uiadvanced_msg_handler);
					libaroma_wm_set_ui_thread(uiadvanced_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
			}
		} return 1;
	}
	//alog("unhandled message=%d state=%d", msg->msg, msg->state);
	return 0;
}
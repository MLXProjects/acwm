#include "recovery.h"

static UI_MENU uibackup_menu;
static char *uibackup_items[]={
	"backup.tosd",
	"backup.fromsd",
	"backup.delsd",
	"backup.advfromsd",
	"backup.freedata",
	"backup.setformat",
	"backup.default"
};

byte uibackup_load(){
	if (!uitools_loadmenu(&uibackup_menu, libaroma_lang_get("backup.title"), UIBACK_ITEMCOUNT, MENU_BACK, uibackup_items, 1)){
		uitools_freemenu(&uibackup_menu);
		return 0;
	}
	return 1;
}

byte uibackup_ui_thread(){
	if (ui_update){
		uitools_drawbg();
		uitools_drawmenu(&uibackup_menu);
		ui_update=0;
		return 1;
	}
	return 0;
}

byte uibackup_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->state!=1 && msg->msg != LIBAROMA_MSG_EXIT) return 0;
	uitools_volarrows(msg);
	/* check message */
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:{
			uitools_freemenu(&uibackup_menu);
			libaroma_wm_set_ui_thread(NULL);
		} return 1;
		case LIBAROMA_MSG_USR(UI_MSG_ACTIVE):{
			uibackup_menu.selected = 0;
			ui_update = 1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLDOWN:{
			if (uibackup_menu.selected>uibackup_menu.itemn-2) uibackup_menu.selected=0;
			else uibackup_menu.selected++;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLUP:{
			if (uibackup_menu.selected<1) uibackup_menu.selected=uibackup_menu.itemn-1;
			else uibackup_menu.selected--;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			switch (uibackup_menu.items[uibackup_menu.selected]->id){
				case BACK_TOSD:{
					libaroma_msg_post(LIBAROMA_MSG_EXIT,0,0,0,0,NULL);
				} break;
				case BACK_FROMSD:{
					
				} break;
				case BACK_ADVFROMSD:{
					
				} break;
				case BACK_FREEDATA:{
					
				} break;
				case BACK_SETFORMAT:{
					
				} break;
				case GENE_BACK:{
					libaroma_wm_set_message_handler(&uimain_msg_handler);
					libaroma_wm_set_ui_thread(uimain_ui_thread);
					libaroma_msg_post(LIBAROMA_MSG_USR(UI_MSG_ACTIVE),1,0,0,0,0);
				} break;
			}
		} return 1;
	}
	//alog("unhandled message %d", msg->msg);
	return 0;
}
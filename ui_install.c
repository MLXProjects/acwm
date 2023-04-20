#include "recovery.h"

static UI_MENU uiinstall_menu;
static char *uiinstall_items[]={
	"install.fromsd",
	"install.fromlast",
	"install.sideload",
	"install.signature"
};

byte uiinstall_load(){
	if (!uitools_loadmenu(&uiinstall_menu, libaroma_lang_get("install.title"), UIINST_ITEMCOUNT, MENU_INST, uiinstall_items, 1)){
		uitools_freemenu(&uiinstall_menu);
		return 0;
	}
	return 1;
}

byte uiinstall_ui_thread(){
	if (ui_update){
		uitools_drawbg();
		uitools_drawmenu(&uiinstall_menu);
		ui_update=0;
		return 1;
	}
	return 0;
}

byte uiinstall_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->state!=1 && msg->msg != LIBAROMA_MSG_EXIT) return 0;
	uitools_volarrows(msg);
	/* check message */
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:{
			uitools_freemenu(&uiinstall_menu);
			libaroma_wm_set_ui_thread(NULL);
		} return 1;
		case LIBAROMA_MSG_USR(UI_MSG_ACTIVE):{
			uiinstall_menu.selected = 0;
			ui_update = 1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLDOWN:{
			if (uiinstall_menu.selected>uiinstall_menu.itemn-2) uiinstall_menu.selected=0;
			else uiinstall_menu.selected++;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLUP:{
			if (uiinstall_menu.selected<1) uiinstall_menu.selected=uiinstall_menu.itemn-1;
			else uiinstall_menu.selected--;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			switch (uiinstall_menu.items[uiinstall_menu.selected]->id){
				case INST_FROMSD:{
					libaroma_msg_post(LIBAROMA_MSG_EXIT,0,0,0,0,NULL);
				} break;
				case INST_FROMLAST:{
					
				} break;
				case INST_FROMSIDE:{
					
				} break;
				case INST_TOGGLESIGN:{
					
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
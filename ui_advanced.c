#include "recovery.h"

static UI_MENU uiadvanced_menu;
static char *uiadvanced_items[]={
	"advanced.recovery",
	"advanced.bootloader",
	"advanced.poweroff",
	"advanced.wipedalvik",
	"advanced.report",
	"advanced.keytest",
	"advanced.showlog"
};
static void uiadvanced_wipedalvik();

byte uiadvanced_load(){
	if (!uitools_loadmenu(&uiadvanced_menu, libaroma_lang_get("advanced.title"), UIADVA_ITEMCOUNT, MENU_ADVA, uiadvanced_items, 1)){
		uitools_freemenu(&uiadvanced_menu);
		return 0;
	}
	return 1;
}

byte uiadvanced_ui_thread(){
	if (ui_update){
		uitools_drawbg();
		uitools_drawmenu(&uiadvanced_menu);
		ui_update=0;
		return 1;
	}
	return 0;
}

byte uiadvanced_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg){
	if (msg->state!=1 && msg->msg != LIBAROMA_MSG_EXIT) return 0;
	uitools_volarrows(msg);
	/* check message */
	switch (msg->msg){
		case LIBAROMA_MSG_EXIT:{
			uitools_freemenu(&uiadvanced_menu);
			libaroma_wm_set_ui_thread(NULL);
		} return 1;
		case LIBAROMA_MSG_USR(UI_MSG_ACTIVE):{
			uiadvanced_menu.selected = 0;
			ui_update = 1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLDOWN:{
			if (uiadvanced_menu.selected>uiadvanced_menu.itemn-2) uiadvanced_menu.selected=0;
			else uiadvanced_menu.selected++;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_VOLUP:{
			if (uiadvanced_menu.selected<1) uiadvanced_menu.selected=uiadvanced_menu.itemn-1;
			else uiadvanced_menu.selected--;
			ui_update=1;
		} return 1;
		case LIBAROMA_MSG_KEY_POWER:
		case LIBAROMA_MSG_KEY_SELECT:{
			switch (uiadvanced_menu.items[uiadvanced_menu.selected]->id){
				case ADVA_RECOVERY:{
					libaroma_msg_post(LIBAROMA_MSG_EXIT,0,0,0,0,NULL);
				} break;
				case ADVA_BOOTLOADER:{
					
				} break;
				case ADVA_POWEROFF:{
					
				} break;
				case ADVA_WIPEDALVIK:{
					uiwipe_setcallback(&uiadvanced_wipedalvik);
					uiwipe_load();
					libaroma_wm_set_message_handler(&uiwipe_msg_handler);
					libaroma_wm_set_ui_thread(uiwipe_ui_thread);
					ui_update=1;
				} break;
				case ADVA_REPORT:{
					
				} break;
				case ADVA_KEYTEST:{
					
				} break;
				case ADVA_SHOWLOG:{
					
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

static void uiadvanced_wipedalvik(){
	alog_ui("Wiping dalvik cache...");
	system("rm -rf /data/dalvik-cache");
}
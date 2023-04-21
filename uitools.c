#include "recovery.h"

int uitools_curmenurows;

byte uitools_loadmenu(UI_MENUP menu, char *title, int item_count, int item_offset, char **items, byte hasback){
	if (hasback){
		item_count++;
	}
	menu->title = libaroma_text(title, UI_TEXTCOLOR, libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE), uires_charheight);
	if (!menu->title){
		alog("failed to allocate menu title");
		return 0;
	}
	menu->items = malloc(sizeof(UI_MENUITEMP)*item_count);
	if (!menu->items){
		alog("failed to allocate uimain menu item list");
		return 0;
	}
	int i;
	for (i=0; i<item_count; i++){
		UI_MENUITEMP item = malloc(sizeof(UI_MENUITEM));
		if (!item){
			alog("failed to allocate main menu item #%d", i);
			uitools_freemenu(menu);
			return 0;
		}
		if (!hasback || i<item_count-1){
			item->text	= libaroma_lang_get(items[i]);
			item->id	= item_offset + i + 1;
		}
		else {
			item->text	= libaroma_lang_get("generic.back");
			item->id	= GENE_BACK;
		}
		menu->items[i] = item;
	}
	menu->itemn = item_count;
	menu->selected = 0;
	return 1;
}

void uitools_freemenu(UI_MENUP menu){
	if (!menu->items) return;
	libaroma_text_free(menu->title);
	menu->title = NULL;
	if (menu->freecb) menu->freecb();
	int i;
	for (i=0; i<menu->itemn; i++){
		alog("freeing items[%d]", i);
		free(menu->items[i]);
	}
	free(menu->items);
	/* reset menu members */
	menu->itemn = 0;
	menu->selected = 0;
	menu->draw_offset = 0;
	menu->freecb = NULL;
}

void uitools_drawbg(){
	/* draw tiled background */
	int w, h, maxw=libaroma_fb()->w, maxh=libaroma_fb()->h;
	for (h=0; h<maxh; h+=uires[UIRES_BG]->h){
		for (w=0; w<maxw; w+=uires[UIRES_BG]->w){
			libaroma_draw(libaroma_fb()->canvas, uires[UIRES_BG], w, h, 0);
		}
	}
	/* draw icon */
	libaroma_draw(libaroma_fb()->canvas, uires[UIRES_CWMLX], 
		(libaroma_fb()->w-uires[UIRES_CWMLX]->w)/2, /* horizontal centered */
		(libaroma_fb()->h-uires[UIRES_CWMLX]->h)/2, /* vertical centered */
	1); /* draw alpha channel */ 
}

void uitools_drawmenu(UI_MENUP menu){
	/* draw title */
	libaroma_text_draw(libaroma_fb()->canvas, menu->title, 0, 0);
	/* draw menu items */
	int i,
		/* title line count */
		title_n = libaroma_text_line_count(menu->title)+1,
		/* max item number on screen = screen height in rows - (title rows + padded line rows & bottom log) */
		max_itemn = uires_maxrows - (title_n + 3),
		/* y offset for item drawing */
		y_offset = uires_charheight*(title_n);
	/* fix max itemn */
	if (menu->itemn < max_itemn){
		max_itemn = menu->itemn;
	}
	uitools_curmenurows = title_n + max_itemn + 2;
	/* fix drawing item offset */
	if (menu->selected < menu->draw_offset){
		menu->draw_offset = menu->selected;
		//alog("max items=%d offset=%d selected=%d", max_itemn, menu->draw_offset, menu->selected);
	}
	else if (menu->selected >= max_itemn){
		if (menu->selected-menu->draw_offset >= max_itemn)
			menu->draw_offset = menu->selected - max_itemn + 1;
		//alog("max items=%d offset=%d selected=%d", max_itemn, menu->draw_offset, menu->selected);
	}
	//alog("title_n=%d, menu->draw_offset=%d, max_itemn=%d, y_offset=%d",
	//		title_n, menu->draw_offset, max_itemn, y_offset);
	for (i=0; i<max_itemn; i++){
		if (i+menu->draw_offset != menu->selected) {
			/* draw normal item */
			//alog(" - %s", menu->items[(i+menu->draw_offset)]->text);
			libaroma_draw_text(libaroma_fb()->canvas, " - ", 0, y_offset, UI_ACCENT, libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE)|LIBAROMA_TEXT_SINGLELINE, uires_charheight);
			libaroma_draw_text(libaroma_fb()->canvas, menu->items[(i+menu->draw_offset)]->text, uires_charwidth*3, y_offset, UI_ACCENT, libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE)|LIBAROMA_TEXT_SINGLELINE, uires_charheight);
		}
		else {
			/* draw selected item background & text */
			//alog(" - %s", menu->items[(i+menu->draw_offset)]->text);
			libaroma_draw_rect(libaroma_fb()->canvas, 0, y_offset, libaroma_fb()->w, uires_charheight+libaroma_dp(1), UI_ACCENT, 0xFF);
			libaroma_draw_text(libaroma_fb()->canvas, " - ", 0, y_offset, UI_TEXTCOLOR, libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE)|LIBAROMA_TEXT_SINGLELINE, uires_charheight);
			libaroma_draw_text(libaroma_fb()->canvas, menu->items[(i+menu->draw_offset)]->text, uires_charwidth*3, y_offset, UI_TEXTCOLOR, libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE)|LIBAROMA_TEXT_SINGLELINE, uires_charheight);
		}
		y_offset += uires_charheight;
	}
	//alog("----------------------");
	/* draw line */
	libaroma_draw_rect(libaroma_fb()->canvas, 0, y_offset + (uires_charheight/2) - libaroma_dp(1), libaroma_fb()->w, libaroma_dp(2), UI_ACCENT, 0xFF);
	/* draw log at bottom */
	if (alog_get()->text) alog_draw();
}

byte uitools_loadlang(char *name){
	libaroma_lang_set("main.title", "CWM-based Recovery v6.0.5.1");
	libaroma_lang_set("main.reboot", "reboot system now");
	libaroma_lang_set("main.install", "install zip");
	libaroma_lang_set("main.wipedata", "wipe data/factory reset");
	libaroma_lang_set("main.wipecache", "wipe cache partition");
	libaroma_lang_set("main.backup", "backup and restore");
	libaroma_lang_set("main.storage", "mounts and storage");
	libaroma_lang_set("main.advanced", "advanced");
	libaroma_lang_set("install.title", "Install update from zip file");
	libaroma_lang_set("install.fromsd", "choose zip from /sdcard");
	libaroma_lang_set("install.fromlast", "choose zip from last install folder");
	libaroma_lang_set("install.sideload", "install zip from sideload");
	libaroma_lang_set("install.signature", "toggle signature verification");
	libaroma_lang_set("backup.title", "Backup and Restore");
	libaroma_lang_set("backup.tosd", "backup to /sdcard");
	libaroma_lang_set("backup.fromsd", "restore from /sdcard");
	libaroma_lang_set("backup.delsd", "delete from /sdcard");
	libaroma_lang_set("backup.advfromsd", "advanced restore from /sdcard");
	libaroma_lang_set("backup.freedata", "free unused backup data");
	libaroma_lang_set("backup.setformat", "choose default backup format");
	libaroma_lang_set("backup.default", "default");
	libaroma_lang_set("storage.title", "Mounts and Storage menu");
	libaroma_lang_set("storage.mount", "mount");
	libaroma_lang_set("storage.unmount", "unmount");
	libaroma_lang_set("storage.format", "format");
	libaroma_lang_set("storage.and", "and");
	libaroma_lang_set("advanced.title", "Advanced Menu");
	libaroma_lang_set("advanced.recovery", "reboot recovery");
	libaroma_lang_set("advanced.bootloader", "reboot to bootloader");
	libaroma_lang_set("advanced.poweroff", "power off");
	libaroma_lang_set("advanced.wipedalvik", "wipe dalvik cache");
	libaroma_lang_set("advanced.report", "report error");
	libaroma_lang_set("advanced.keytest", "key test");
	libaroma_lang_set("advanced.showlog", "show log");
	libaroma_lang_set("wipe.title", "Confirm wipe?");
	libaroma_lang_set("wipe.title_data", "Confirm wipe of all user data?");
	libaroma_lang_set("wipe.title_format", "Confirm format?");
	libaroma_lang_set("wipe.warning", "THIS CAN NOT BE UNDONE.");
	libaroma_lang_set("wipe.yes", "Yes - Wipe");
	libaroma_lang_set("wipe.yes_format", "Yes - Format");
	libaroma_lang_set("wipe.yes_all", "Yes - Wipe all user data");
	libaroma_lang_set("wipe.yes_cache", "Yes - Wipe Cache");
	libaroma_lang_set("wipe.yes_dalvik", "Yes - Wipe Dalvik Cache");
	libaroma_lang_set("wipe.formatting", "Formatting");
	libaroma_lang_set("generic.back", "+++++Go Back+++++");
	libaroma_lang_set("generic.done", "Done.");
	libaroma_lang_set("generic.yes", "Yes");
	libaroma_lang_set("generic.no", "No");
	return 1;
}
#ifndef __uitools_h__
#define __uitools_h__

/* ui menu types */
typedef struct {
	char *text;
	byte id;
} UI_MENUITEM, *UI_MENUITEMP;

typedef struct {
	LIBAROMA_TEXT title;
	UI_MENUITEMP *items;
	int itemn;
	byte selected;
	int draw_offset;
	void (*freecb)();
} UI_MENU, *UI_MENUP;


/* quick & dirty patch to use keyboard arrows as vol keys and enter as select */
#define uitools_volarrows(msg) \
	{\
		if (msg->key==LIBAROMA_HID_KEY_UP) msg->msg=LIBAROMA_MSG_KEY_VOLUP;\
		if (msg->key==LIBAROMA_HID_KEY_DOWN) msg->msg=LIBAROMA_MSG_KEY_VOLDOWN;\
	}

/* current menu height in rows (including bottom line & top/bottom padding) */
extern int uitools_curmenurows;

/* ui tools */
extern byte uitools_loadmenu(UI_MENUP menu, char *text, int item_count, int item_offset, char **items, byte hasback);
extern void uitools_freemenu(UI_MENUP menu);
extern void uitools_drawbg();
extern void uitools_drawmenu(UI_MENUP menu);
extern byte uitools_loadlang(char *name);

#endif /*__uitools_h__*/
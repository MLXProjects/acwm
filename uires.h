#ifndef __uires_h__
#define __uires_h__

/* ui resources count */
#define UI_RESCOUNT	23

/* ui resource IDs */
enum UIRES_IMAGES {
	UIRES_CID,
	UIRES_BG,
	UIRES_CWMLX,
	UIRES_ICON,
	UIRES_INSTALL,
	UIRES_INST01,
	UIRES_INST02,
	UIRES_INST03,
	UIRES_INST04,
	UIRES_INST05,
	UIRES_INST06,
	UIRES_INST07,
	UIRES_ERROR,
	UIRES_PROGEMPTY,
	UIRES_PROG01,
	UIRES_PROG02,
	UIRES_PROG03,
	UIRES_PROG04,
	UIRES_PROG05,
	UIRES_PROG06,
	UIRES_PROGFULL,
	UIRES_FWINSTALL,
	UIRES_FWERROR
};

/* default settings */
#define UI_ACCENT		RGB(00BFFF)
#define UI_DEBUGCOLOR	RGB(FF0000)
#define UI_TEXTCOLOR	RGB(FFFFFF)
#define UI_FONTSIZE		4

/* custom menu messages */
#define UI_MSG_ACTIVE	0x01

/* menu item counts */
#define UIMAIN_ITEMCOUNT	7
#define UIINST_ITEMCOUNT	4
#define UIBACK_ITEMCOUNT	6
#define UIADVA_ITEMCOUNT	7
#define UIWIPE_ITEMCOUNT	2
/* UISTOR is dynamic and UIGENE has no dedicated menu */

/* prefixes needed to find out UISTOR item actions */
#define UISTOR_MOUNT		150
#define UISTOR_FORMAT		50

/* menu item identifiers
 * MENU_ : menu header (used as base)
 * MAIN_ : main menu
 * INST_ : install zip menu
 * BACK_ : backup/restore menu
 * STOR_ : mounts/storage menu
 * ADVA_ : advanced menu
 * GENE_ : generic options (any menu)
 */
enum MENU_PAGESANDITEMS {
	MENU_MAIN,
	MAIN_REBOOT,
	MAIN_INSTALL,
	MAIN_WIPEDATA,
	MAIN_WIPECACHE,
	MAIN_BACKREST,
	MAIN_MOUNTS,
	MAIN_ADVANCED,
	MENU_INST,
	INST_FROMSD,
	INST_FROMLAST,
	INST_FROMSIDE,
	INST_TOGGLESIGN,
	MENU_BACK,
	BACK_TOSD,
	BACK_FROMSD,
	BACK_DELSD,
	BACK_ADVFROMSD,
	BACK_FREEDATA,
	BACK_SETFORMAT,
	MENU_STOR,
	STOR_MOUNT,
	STOR_UNMOUNT,
	STOR_FORMAT,
	MENU_ADVA,
	ADVA_RECOVERY,
	ADVA_BOOTLOADER,
	ADVA_POWEROFF,
	ADVA_WIPEDALVIK,
	ADVA_REPORT,
	ADVA_KEYTEST,
	ADVA_SHOWLOG,
	MENU_GENE,
	GENE_NO,
	GENE_YES,
	GENE_BACK
};

/* global resources */
extern int uires_charwidth;
extern int uires_charheight;
extern int uires_maxrows;
extern LIBAROMA_CANVASP uires[UI_RESCOUNT];

#endif /*__uires_h__*/
#ifndef __recovery_h__
#define __recovery_h__

#include <stdio.h>
#include <string.h>
#include <aroma.h>
#include "log.h"
#include "partmgr.h"
#include "uires.h"
#include "uitools.h"
#include "ui_wipe.h"

/* some defaults */
#define RECOVERY_VER		"0.1.0"
#define RECOVERY_API		3
#define UPDATER_PATH		"/tmp/updater"
#define LOG_PATH			"/tmp/updater"

/* global variables */
extern LIBAROMA_ZIP res_zip;
extern byte ui_update;

/* page functions */
extern byte uimain_load();
extern byte uimain_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
extern byte uimain_ui_thread();
extern byte uiinstall_load();
extern byte uiinstall_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
extern byte uiinstall_ui_thread();
extern byte uibackup_load();
extern byte uibackup_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
extern byte uibackup_ui_thread();
extern byte uistorage_load();
extern byte uistorage_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
extern byte uistorage_ui_thread();
extern byte uiadvanced_load();
extern byte uiadvanced_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
extern byte uiadvanced_ui_thread();
extern byte uiwipe_load();
extern byte uiwipe_msg_handler(LIBAROMA_WMP wm, LIBAROMA_MSGP msg);
extern byte uiwipe_ui_thread();

#endif /*__recovery_h__*/
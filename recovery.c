#include "recovery.h"

LIBAROMA_ZIP res_zip;
byte ui_update = 1;

int main(int argc, char **argv){
	/* print 1st/3rd argument and load resource zip file */
	alog("argc=%d argv[%d]=%s", argc, (argc>3)?3:1, (argc>1)?((argc>3)?argv[3]:argv[1]):"null");
	/* if +3 arguments, probably 4th is zip path */
	if (argc>3) res_zip = libaroma_zip(argv[3]);
	/* if zip not loaded and more than 1 argument, 2nd must be zip path */
	if (argc>1 && !res_zip) res_zip = libaroma_zip(argv[1]);
	/* if zip not loaded try some hardcoded paths like current dir & /tmp */
	if (!res_zip) res_zip = libaroma_zip("./res.zip");
	if (!res_zip) res_zip = libaroma_zip("/tmp/res.zip");
	if (!res_zip){
		/* okay let's give up */
		alog("failed to open zip, F");
		return 1;
	}
	if (argc>3) libaroma_config()->runtime_monitor = LIBAROMA_START_MUTEPARENT;
	/* force smol res for desktop testing */
	libaroma_sdl_startup_size(240, 320);
	/* start libaroma */
	if (!libaroma_start()){
		alog("failed to start libaroma");
		libaroma_zip_release(res_zip);
		return 2;
	}
	/* set low dpi if smol res */
	if (libaroma_fb()->w <= 240 || 
			libaroma_fb()->h <=320){
		libaroma_fb()->dpi = 120;
	}
	int ret = 0;
	alog("libaroma started");
	/* load font */
	if (!libaroma_font(0, libaroma_stream_mzip(res_zip, "Deja Vu Sans Mono.ttf"))){
		alog("failed to load font");
		ret = 3;
		goto main_end;
	}
	/* preload main resources */
	if (!uires_preload()){
		alog("failed to preload resources");
		ret = 4;
		goto main_end;
	}
	/* load language */
	uitools_loadlang("english");
	/* load static pages */
	if (!uimain_load()){
		ret = 5;
		goto main_end;
	}
	if (!uiinstall_load()){
		ret = 6;
		goto main_end;
	}
	if (!uibackup_load()){
		ret = 7;
		goto main_end;
	}
	if (!uiadvanced_load()){
		ret = 8;
		goto main_end;
	}
	/* set initial message handler & ui thread */
	libaroma_wm_set_message_handler(&uimain_msg_handler);
	libaroma_wm_set_ui_thread(&uimain_ui_thread);
	/* wait until window manager is stopped
	 * is there a proper way to do this?
	 * idk and i'm the main libaroma developer, i guess i'm screwed */
	do { libaroma_sleep(100); } while(libaroma_wm()->client_started);
	/* cleanup */
main_end:
	/* if failed after resource loading or didn't fail at all, release resources */
	if (!ret || ret>3) uires_cleanup();
	/* release resource zip & exit libaroma */
	libaroma_zip_release(res_zip);
	libaroma_end();
	/* return exit code, greather than zero means something failed */
	return ret;
}
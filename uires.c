#include "recovery.h"

/* ui resources */
int uires_charwidth;
int uires_charheight;
int uires_maxrows;
LIBAROMA_CANVASP uires[UI_RESCOUNT];
static const char *uires_paths[UI_RESCOUNT] = {
	"images/icon_cid.png",
	"images/stitch.png",
	"images/icon_cwmlx.png",
	"images/icon_clockwork.png",
	"images/icon_installing.png",
	"images/icon_installing_overlay01.png",
	"images/icon_installing_overlay02.png",
	"images/icon_installing_overlay03.png",
	"images/icon_installing_overlay04.png",
	"images/icon_installing_overlay05.png",
	"images/icon_installing_overlay06.png",
	"images/icon_installing_overlay07.png",
	"images/icon_error.png",
	"images/progress_empty.png",
	"images/indeterminate01.png",
	"images/indeterminate02.png",
	"images/indeterminate03.png",
	"images/indeterminate04.png",
	"images/indeterminate05.png",
	"images/indeterminate06.png",
	"images/progress_fill.png",
	"images/icon_firmware_install.png",
	"images/icon_firmware_error.png"
};

byte uires_preload(){
	/* get tallest char height */
	LIBAROMA_TEXT sample = libaroma_text("B", 0, 
		libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE)|LIBAROMA_TEXT_SINGLELINE, 0);
	uires_charwidth = libaroma_text_width(sample);
	uires_charheight = libaroma_text_height(sample) + 2;
	alog("char width=%d, height=%d", uires_charwidth, uires_charheight);
	libaroma_text_free(sample);
	/* calculate max screen rows */
	uires_maxrows = (libaroma_fb()->h/uires_charheight);
	/* may this ever happen? let's be safe */
	if (uires_maxrows<1) uires_maxrows = 1;
	/* load & scale resources */
	byte scale_res = (libaroma_fb()->dpi!=160)?1:0;
	alog("resource scaling %sabled", (scale_res)?"en":"dis");
	int i;
	for (i=0; i<22; i++){
		LIBAROMA_CANVASP resource = libaroma_image_mzip(res_zip, uires_paths[i], 0);
		if (!resource){
			alog("failed to load resource %s", uires_paths[i]);
			return 0;
		}
		if (scale_res){
			uires[i]=libaroma_canvas_alpha(libaroma_dp(resource->w), libaroma_dp(resource->h));
			if (uires[i]){
				/* scale and continue loop */
				libaroma_draw_scale_smooth(uires[i], resource, 
					0, 0, uires[i]->w, uires[i]->h,
					0, 0, resource->w, resource->h);
				libaroma_canvas_free(resource);
				continue;
			}
			else {
				alog("failed to allocate scaled resource, using unscaled image");
			}
		}
		/* fallback to unscaled resource */
		uires[i]=resource;
	}
	return 1;
}

void uires_cleanup(){
	int i;
	for (i=21; i>=0; i--){
		if (!uires[i]) continue;
		libaroma_canvas_free(uires[i]);
	}
}
#include "recovery.h"

static RECOVERY_LOG _alog;

RECOVERY_LOGP alog_get(){
	return &_alog;
}

void alog_ui(char *text){
	if (_alog.parsed_text){
		libaroma_text_free(_alog.parsed_text);
		_alog.parsed_text = NULL;
	}
	_alog.text = text;
	ui_update = 1;
}

void alog_draw(){
	/* parse text if needed */
	if (!_alog.parsed_text){
		LIBAROMA_TEXT parsed = libaroma_text(_alog.text, UI_TEXTCOLOR, 
			libaroma_fb()->w, LIBAROMA_FONT(0,UI_FONTSIZE), uires_charheight);
		if (!parsed){
			alog("failed to parse log text");
			return 0;
		}
		_alog.parsed_text = parsed;
	}
	int log_rows = libaroma_text_line_count(_alog.parsed_text),
		log_maxrows = uires_maxrows - uitools_curmenurows,
		line_offset = 0,
		draw_offset = uires_maxrows - log_rows;
	if (log_rows > log_maxrows) line_offset = log_rows - log_maxrows;
	libaroma_draw_rect(libaroma_fb()->canvas,
		0, draw_offset*uires_charheight,
		libaroma_fb()->w, log_rows*uires_charheight,
		UI_DEBUGCOLOR, 0x0);
	libaroma_text_draw(libaroma_fb()->canvas, _alog.parsed_text, 0, draw_offset*uires_charheight);
}
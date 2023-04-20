#ifndef __log_h__
#define __log_h__

typedef struct {
	char *text;
	LIBAROMA_TEXT parsed_text;
	int draw_line;
} RECOVERY_LOG, * RECOVERY_LOGP;

/* write to log */
#define alog(...) { printf(__VA_ARGS__); printf("\n"); }

/* log utils */
extern RECOVERY_LOGP alog_get();
extern void alog_ui(char *text);
extern void alog_parse();

#endif /*__log_h__*/
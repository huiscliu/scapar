
#ifndef SCAPAR_UTILS_H
#define SCAPAR_UTILS_H

#include "scapar-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* mem */
void * scapar_malloc(size_t size);
void * scapar_calloc(size_t nmemb, size_t size);
void * scapar_realloc(void *ptr, size_t size);
void scapar_free(void *ptr);

/* log */
int  scapar_warning(const char *fmt, ...);
void scapar_error(int code, const char *fmt, ...);
int  scapar_printf(const char *fmt, ...);
void scapar_pointer_check(const char *func, const void *key, const char *kn);

char * scapar_fgets(char *s, int size, FILE *stream, size_t *line);
SCAPAR_FLT scapar_atof(const char *ptr);
SCAPAR_INT scapar_atoi(const char *ptr);
int scapar_check_if_discard(const char *b, char c);
void scapar_str_remove_spaces(char *str);

/* input is comment: 1 is true and 0 is false */
int scapar_check_input_is_comment(const char *b, char cmt);

/* input is section ending: 1 is true and 0 is false */
int scapar_check_input_is_section_ending(const char *b, char *cmt);

/* input is empty: 1 is true and 0 is false */
int scapar_check_input_is_empty(const char *b);

/* upper case to lower case */
void scapar_string_to_lower(char *str);

int scapar_item_key_comp(const void *p, const void *n);

void scapar_magic_check(const SCAPAR *scapar);

void scapar_info_init(SCAPAR_INFO *in);
SCAPAR_INFO * scapar_info_copy(SCAPAR_INFO *in);
void scapar_info_destroy(SCAPAR_INFO *in);

/* get time */
SCAPAR_FLT scapar_get_time(void);

/* get mem */
SCAPAR_FLT scapar_get_mem(void);

#ifdef __cplusplus
}
#endif

#endif
